#pragma once

#define MAX_GAMEOBJECT 1000


template<class T>
struct ComponentCount { size_t Count = MAX_GAMEOBJECT; };

#include <typeindex>
#include "ObjectPool.hpp"
#include "Singleton.hpp"
#include <unordered_map>

class ModelLoader;
class Scene;
class Component;
class ParticleFactory;
class AudioManager;
class LightLoader;
class SpriteFactory;

class ComponentLake : public CommonUtilities::Singleton<ComponentLake>
{
	friend class  CommonUtilities::Singleton<ComponentLake>;
public:
	ComponentLake();
	~ComponentLake();

	void PrepareObjectsInPools(
		Scene* aScene, 
		ParticleFactory* aParticleFactory, 
		AudioManager* aAudioManager, 
		LightLoader* aLightLoader,
		std::vector<Entity*>* aEntityVector,
		CommonUtilities::ObjectPool<Entity>* aEntityPool,
		SpriteFactory* aSpriteFactory);

	void RegisterComponents();
	bool UpdateComponents(float aDeltaTime);

	template<class T>
	void RegisterComponent();

	template<class T>
	T* RetrieveComponent();

	template<class T>
	void ReturnComponent(Component* aComponent);
	void ReturnComponent(std::type_index aComponentIndex, Component* aComponent);


	void DisableActiveComponent(std::type_index aComponentType, Component* aComponent);
	void EnableActiveComponent(std::type_index aComponentType, Component* aComponent);


	//std::vector<Collision*> GetCollisionComponentVector();

private:
	int GetComponentTypeIndex(std::type_index aTypeIndex);

	std::unordered_map<std::type_index, CommonUtilities::DisposeAcceptor*> myComponentPools;

	std::vector<std::vector<Component*>> myAttachedActiveComponents;
	std::vector<int> myActiveComponentIndex;

	std::vector<int> myUpdateOrder;
	std::unordered_map<std::type_index, int> myComponentTypeIndexLookupTable;
	std::unordered_map<std::string, size_t> myComponentPoolSize;

	//std::vector<Collision*> myCollisionComponents;	
};

inline std::unordered_map<const char*, std::pair<unsigned int, unsigned int>>& GetComponentsInUse()
{
	static std::unordered_map<const char*, std::pair<unsigned int, unsigned int>> components;
	return components;
}

template<class T>
inline void ComponentLake::RegisterComponent()
{
	myComponentPools[std::type_index(typeid(T))] = new CommonUtilities::ObjectPool<T>;
	size_t count = MAX_GAMEOBJECT;
	auto it = myComponentPoolSize.find(typeid(T).name());
	if (it != myComponentPoolSize.end())
	{
		count = it->second;
	}
	((CommonUtilities::ObjectPool<T>*)myComponentPools[std::type_index(typeid(T))])->Init(count);
	myUpdateOrder.push_back(CAST(int, myUpdateOrder.size()));
	myComponentTypeIndexLookupTable[std::type_index(typeid(T))] = CAST(int, myUpdateOrder.size() - 1);
	myActiveComponentIndex.push_back(0);
	myAttachedActiveComponents.resize(myUpdateOrder.size());
#if TRACKCOMPONENTUSAGE
	GetComponentsInUse()[typeid(T).name()].second = ((CommonUtilities::ObjectPool<T>*)myComponentPools[std::type_index(typeid(T))])->Size();
#endif // TRACKCOMPONENTUSAGE
}


template<class T>
inline T* ComponentLake::RetrieveComponent()
{
	if (myComponentPools.find(typeid(T)) == myComponentPools.end())
		RegisterComponent<T>();

	Component* component = (Component*)(((CommonUtilities::ObjectPool<T>*)myComponentPools[std::type_index(typeid(T))]))->Retrieve();

	if (!component)
	{
		SYSERROR("Out of components if this is too low increase the limit in ComponentLake.cpp", std::string(typeid(T).name() + 6), std::to_string(myComponentPoolSize[typeid(T).name()]));
	}

#if TRACKCOMPONENTUSAGE
	GetComponentsInUse()[typeid(T).name()].first++;
#endif // TRACKCOMPONENTUSAGE

	component->myType = typeid(T).name() + 6;
	component->myTypeIndex = typeid(T);

	component->myEnabled = true;

	myAttachedActiveComponents[GetComponentTypeIndex(std::type_index(typeid(T)))].push_back(component);
	return (T*)component;
}

template<class T>
inline void ComponentLake::ReturnComponent(Component* aComponent)
{
	ReturnComponent(typeid(T), aComponent);
}