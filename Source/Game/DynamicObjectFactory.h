#pragma once
#include "Observer.hpp"
#include "Publisher.hpp"
#include "Vector3.hpp"
#include <vector>
#include "ObjectPoolSizes.h"

namespace CommonUtilities
{
	template <typename Type>
	class ObjectPool;
}

class GBPhysX;
class Octree;
class Entity;
class ComponentLake;

class DynamicObjectFactory : public Observer, public Publisher
{
public:

	DynamicObjectFactory();
	~DynamicObjectFactory();

	void Init(GBPhysX* aGBPhysX, Octree* aOctTree, std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt);
	void CreateDynamicObject(std::string aFilePath, CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aRot, CommonUtilities::Vector3<float> aScale);
	void SetPlayerPtr(Entity* aPlayerPtr);
private:
	virtual void RecieveMessage(const Message& aMessage) override;

	GBPhysX* myGBPhysX;
	Octree* myOctree;
	std::vector<Entity*>* myEntityVector;
	std::map<std::string, std::vector<Entity*>*> myLevelObjectMapping;
	std::vector<Entity*>* myCurrentLevel;
	CommonUtilities::ObjectPool<Entity>* myEntityPool;
	unsigned int* myEntityIDInt;
	Entity* myPlayerPtr = nullptr;
};

