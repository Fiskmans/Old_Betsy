#pragma once
#include <vector>
#include "ComponentLake.h"
//
#include <array>
#include "Vector.hpp"
#include "Matrix4x4.hpp"
#include "EmilsEnums.h"
#if USEIMGUI
#include <map>
#include "ImGuiPackage.h"
#endif // !_RETAIL
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
	void Spawn(const CommonUtilities::Vector3<float>& aPosition); //Rotation?
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
	
	void SetSpawnPos(CommonUtilities::Vector3<float> aSpawnPos);

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

	CommonUtilities::Vector3<float> GetPosition();
	void SetPosition(CommonUtilities::Vector3<float> aPosition);

	CommonUtilities::Matrix4x4<float> GetRotation();
	void SetRotation(CommonUtilities::Vector3<float> aRotation);
	void SetRotation(CommonUtilities::Matrix4x4<float> aRotationMatrix);

	void SetSavedRotationValues(CommonUtilities::Vector3<float> aRot);
	CommonUtilities::Vector3<float> GetSavedRotationValues();

	CommonUtilities::Vector3<float> GetScale();
	void SetScale(CommonUtilities::Vector3<float> aScale);

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

	CommonUtilities::Vector3<float> myPosition;
	CommonUtilities::Vector3<float> myScale;

	EntityType myEntityType;
	int myDestructibleType;

	int myEntityID;
	unsigned int myNodeID;

	size_t myActiveThings;

	bool myIsAlive;
	bool myIsMoving;

	long long mySpawnTime;

	CommonUtilities::Vector3<float> mySpawnPos;
	CommonUtilities::Vector3<float> mySavedRotationValues;
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