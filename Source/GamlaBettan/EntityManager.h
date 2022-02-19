#pragma once

#include "CommonUtilities\Singleton.hpp"

#include "GamlaBettan\Entity.h"

class Scene
{
public:
	Scene() = default;
	Scene(const std::vector<EntityID>& aEntities) : myEntities(aEntities) {}
	~Scene() = default;

	std::vector<EntityID> myEntities;
};

class EntityManager : public CommonUtilities::Singleton<EntityManager>
{
public:
	EntityID MakeEntity();
	Entity* Retrieve(EntityID aID);
	void Return(EntityID& aID);

#if USEIMGUI
	void ImGui();
	std::vector<EntityID> GetAllActive();
#endif
	void SaveScene(Scene& aScene, const std::string& aFilePath);
	void SaveCollection(const std::vector<EntityID>& aEntities, FiskJSON::ArrayWrapper aObject);
	[[nodiscard]] Scene LoadScene(const std::string& aFilePath);
	[[nodiscard]] std::vector<EntityID> LoadCollection(const FiskJSON::Object& aObject);

private:
	static const size_t myBatchSize = 256;
	static const EntityID myIDStartOffset = 1024;

	std::vector<EntityID> myUnused;
	std::vector<std::array<Entity, myBatchSize>*> myPools;
};

