#pragma once
#include "ComponentLake.h"

#include "EntityMessage.h"
#include "Enums.h"
#include "Component.h"

class ComponentLake;
class CCamera;
class Item;


struct LoadData 
{
	float x;
	float y;
};

class Entity
{
public:
	Entity();
	~Entity();

	struct ParticlePackage
	{
		ParticlePackage(const std::string& aName, float aSize, V3F aOffset = { 0,0,0 }) { myName = aName; mySize = aSize; myOffset = aOffset; }
		std::string myName;
		float mySize;
		V3F myOffset;
	};

	void Init(EntityType aEntityType, unsigned int aEntityID, unsigned int aNodeID = 0);

	void SetEntityType(EntityType aEntityType);
	void Spawn();
	void Spawn(const V3F& aPosition); //Rotation?
	void Update(const float aDeltaTime);

	void SendEntityMessage(EntityMessage aMessage, void* someData = nullptr);


	template<class T>
	T* AddComponent();

	template<class T>
	void RemoveComponent();
	void RemoveAllComponents();

	template<class T>
	T* GetComponent();

	void DisableAll();
	void EnableAll();

	void Dispose();

	void SetSpawnPos(V3F aSpawnPos);

	bool GetIsAlive();
	bool GetShouldBeRemoved();
	void SetIsAlive(const bool aIsAlive);
	void Kill();

	V3F GetForward();
	V3F GetBack();
	V3F GetUp();
	V3F GetDown();
	V3F GetLeft();
	V3F GetRight();

	bool GetIsMoving();
	void SetIsMoving(const bool aIsMoving);

	V3F GetPosition();
	void SetPosition(V3F aPosition);

	CommonUtilities::Matrix4x4<float> GetRotation();
	void SetRotation(V3F aRotation);
	void SetRotation(CommonUtilities::Matrix4x4<float> aRotationMatrix);

	void SetSavedRotationValues(V3F aRot);
	V3F GetSavedRotationValues();

	V3F GetScale();
	void SetScale(V3F aScale);

	EntityType GetEntityType();
	int GetEntityID();

	void SetDestructibleType(const int aType);
	int GetDestructibleType();

	void SetTargetPosition(V3F aPosition);

	unsigned int GetNodeID() const;

#if USEIMGUI
	void ImGuiNode(ImGuiNodePackage& aPackage);
#endif // !_RETAIL

	void AddActivity();
	void FinishActivity();

	void SetSpawnTime(long long aTime);
	long long GetSpawnTime();

	int myInteractableID;

	void InteractWith(Item* aHeldTool);
	V3F myInteractPoint;

	void MoveToSpawn();
private:
	std::unordered_map<std::type_index, Component*> myComponents;
	CommonUtilities::Matrix4x4<float> myRotationMatrix;

	V3F myPosition;
	V3F myScale;

	EntityType myEntityType;
	int myDestructibleType;

	int myEntityID;
	unsigned int myNodeID;

	size_t myActiveThings;

	bool myIsAlive;
	bool myIsMoving;

	long long mySpawnTime;

	V3F mySpawnPos;
	V3F mySavedRotationValues;
};


template<class T>
inline T* Entity::AddComponent()
{
	T* ptr = ComponentLake::GetInstance().RetrieveComponent<T>();
	if (!ptr)
	{
		ptr = new T();
		SYSWARNING("out of component new'ing", typeid(T).name());
	}
	myComponents[std::type_index(typeid(T))] = (Component*)(ptr);
	myComponents[std::type_index(typeid(T))]->SetParent(this);

	((Component*)(ptr))->OnAttach();

	return ptr;
}

template<class T>
inline void Entity::RemoveComponent()
{
	myComponents[std::type_index(typeid(T))]->OnDetach();
	ComponentLake::GetInstance().ReturnComponent<T>(myComponents[std::type_index(typeid(T))]);
	myComponents.erase(std::type_index(typeid(T)));
}

template<class T>
inline T* Entity::GetComponent()
{
	if (myComponents.find(std::type_index(typeid(T))) == myComponents.end())
		return nullptr;

	return (T*)(myComponents[std::type_index(typeid(T))]);
}