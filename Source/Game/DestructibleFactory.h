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
class CharacterData;

class DestructibleFactory : public Observer
{
public:

	DestructibleFactory();
	~DestructibleFactory();

	void Init(Octree* aOctTree, std::vector<Entity*>* aEntityVector, CommonUtilities::ObjectPool<Entity>* aEntityPool, ComponentLake* aComponentLake, unsigned int* aEntityIDInt, CharacterData* aCharacterData);
	void CreateDestructibleObject(std::string aFilePath, CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aRot, CommonUtilities::Vector3<float> aScale, int aType);

private:
	virtual void RecieveMessage(const Message& aMessage) override;
	
	Octree* myOctree;
	std::vector<Entity*>* myEntityVector;
	CommonUtilities::ObjectPool<Entity>* myEntityPool;
	unsigned int* myEntityIDInt;
	CharacterData* myCharacterData;
};