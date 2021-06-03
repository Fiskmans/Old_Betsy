#pragma once
#include "Observer.hpp"
#include "ObjectPoolSizes.h"

namespace CommonUtilities
{
	template <typename Type>
	class ObjectPool;
}
class Octree;
class Entity;
class ComponentLake;

class PointLightFactory : public Observer
{

public:

	PointLightFactory();
	~PointLightFactory();

	void Init(std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt);
	void CreatePointLight(CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aColor, float aIntensity, float aPeriod, float aMinPercent, float aRange, int id);
	
private:

	virtual void RecieveMessage(const Message& aMessage) override;

	std::vector<Entity*>* myEntityVector;
	CommonUtilities::ObjectPool<Entity>* myEntityPoolPtr;
	unsigned int* myEntityIDInt;
	ComponentLake* myComponentLake;
};