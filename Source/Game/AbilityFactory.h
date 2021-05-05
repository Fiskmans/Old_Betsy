#pragma once
#include "Observer.hpp"
#include "Vector3.hpp"
#include <vector>
#include "ObjectPoolSizes.h"


namespace CommonUtilities
{
	template <typename Type>
	class ObjectPool;
}
class Octree;
class Entity;
class ComponentLake;
class AbilityData;

class AbilityFactory :
	public Observer
{
public:

	AbilityFactory();
	~AbilityFactory();

	void Init(std::vector<Entity*>* aEntityVector, std::vector<Entity*>* aAbilityVFXVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt, AbilityData* aAbilityData);
	void CreateAbility(Entity* aCastingEntity, V3F aDirection, int aAbilityType = 0);
	Entity* CreateAbility(V3F aPosition, V3F aDirection, int aAbilityType);
	void CreateAbility(Entity* aCastingEntity, V3F aPosition, int aAbilityType, bool aVal);
	void SetPlayerPtr(Entity* aPlayerPtr);
private:
	virtual void RecieveMessage(const Message& aMessage) override;
	std::vector<Entity*>* myEntityVector;
	std::vector<Entity*>* myAbilityVFXVector;
	CommonUtilities::ObjectPool<Entity>* myEntityPoolPtr;
	unsigned int* myEntityIDInt;
	ComponentLake* myComponentLake;
	AbilityData* myAbilityData;
	Entity* myPlayerPtr;
};

