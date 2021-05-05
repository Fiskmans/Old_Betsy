#pragma once
#include "Observer.hpp"
#include "Vector3.hpp"
#include <vector>
#include "ObjectPoolSizes.h"
#include "Spline.h"

namespace CommonUtilities
{
	template <typename Type>
	class ObjectPool;
}
class Octree;
class Entity;
class ComponentLake;

class TriggerBoxFactory : public Observer
{
public:

	TriggerBoxFactory();
	~TriggerBoxFactory();

	void Init(Octree* aOctTree, std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt);
	void CreateTriggerBox(std::string aFilePath, int aTriggerBoxID, int aEventID, CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aRot, CommonUtilities::Vector3<float> aScale, bool aIsStrikeable);

	static void DrawTriggerBoxes();

private:
	virtual void RecieveMessage(const Message& aMessage) override;

	Octree* myOctree;
	std::vector<Entity*>* myEntityVector;
	static std::vector<Entity*> myDebugVector;
	CommonUtilities::ObjectPool<Entity>* myEntityPool;
	unsigned int* myEntityIDInt;
	ComponentLake* myComponentLake;
	std::vector<Spline<CU::Vector3<float>>>* mySplines;
};

