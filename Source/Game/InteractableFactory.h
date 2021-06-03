#pragma once
#include "Observer.hpp"
#include "Publisher.hpp"
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

class InteractableFactory : public Observer, public Publisher
{
public:

	InteractableFactory();
	~InteractableFactory();

	void Init(GBPhysX* aGBPhysX, Octree* aOctTree, std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt);
	void CreateInteractableObject(std::string aFilePath, int aInteractID, CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aRot, CommonUtilities::Vector3<float> aScale, V3F aInteractPoint, std::string aImagePath);

private:
	virtual void RecieveMessage(const Message& aMessage) override;

	GBPhysX* myGBPhysX;
	Octree* myOctree;
	std::vector<Entity*>* myEntityVector;
	std::map<std::string, std::vector<Entity*>*> myLevelObjectMapping;
	std::vector<Entity*>* myCurrentLevel;
	CommonUtilities::ObjectPool<Entity>* myEntityPool;
	unsigned int* myEntityIDInt;
};