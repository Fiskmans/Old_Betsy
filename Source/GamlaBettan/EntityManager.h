#pragma once

#include "CommonUtilities\Singleton.hpp"

#include "GamlaBettan\Entity.h"

class EntityManager : public CommonUtilities::Singleton<EntityManager>
{
public:
	EntityID Get();
	Entity* Retrieve(EntityID aID);
	void Return(EntityID& aID);

#if USEIMGUI
	void ImGui();
#endif

private:
	static const size_t myBatchSize = 256;
	static const EntityID myIDStartOffset = 1024;

	std::vector<EntityID> myUnused;
	std::vector<std::array<Entity, myBatchSize>*> myPools;
};

