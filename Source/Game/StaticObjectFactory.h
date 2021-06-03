#pragma once
#include "Observer.hpp"
#include "Publisher.hpp"
#include "ObjectPoolSizes.h"
#include "Spline.h"
#include "UIManager.h"

namespace CommonUtilities
{
	template <typename Type>
	class ObjectPool;
}

class GBPhysX;
class Octree;
class Entity;
class ComponentLake;

class StaticObjectFactory : public Observer, public Publisher
{
public:

	StaticObjectFactory();
	~StaticObjectFactory();

	void Init(GBPhysX* aGBPhysX, Octree* aOctTree, std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt, UIManager* aUIManager);
	void CreateStaticObject(std::string aFilePath, CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aRot, CommonUtilities::Vector3<float> aScale);

private:
	virtual void RecieveMessage(const Message& aMessage) override;

	GBPhysX* myGBPhysX;
	Octree* myOctree;
	std::vector<Entity*>* myEntityVector;
	std::map<std::string, std::vector<Entity*>*> myLevelObjectMapping;
	std::vector<Entity*>* myCurrentLevel;
	CommonUtilities::ObjectPool<Entity>* myEntityPool;
	unsigned int* myEntityIDInt;
	UIManager* myUIManager;
};

