#pragma once

#include "DataStructs.h"

class Asset;

struct LevelParseResult
{
	std::vector<StaticObjectInstanceBuffer> myStaticObjects;
	std::vector<InteractableInstanceBuffer> myInteractables;
	std::vector<EnemyInstanceBuffer> myEnemyObjects;
	std::vector<TriggerBoxInstanceBuffer> myTriggerBuffer;
	std::vector<std::pair<int, V3F>> myMayaPositions;
	std::vector<PointLightInstanceBuffer> myPointLights;
	std::vector<PhysXBoxInstanceBuffer> myCollsionBoxes;
	std::vector<DestructibleObjectInstanceBuffer> myDestrucables;
	std::vector<SpotLightBuffer> mySpotlights;
	std::vector<DecalBuffer> myDecals;


	std::string myLevelFile;
	std::string mySkyboxPath;
	int myWorldAxisSize;
	DirLightInstanceBuffer myEnvironmentlight;
	std::string myNavMeshPath;
	V3F myPlayerStart;

	size_t myCounter; //Metadata that should be moved
	size_t myStep;
	bool myIsComplete = true;
	bool myIsSuccessfull = false;
};

class LevelLoader
{
private:
	friend class AssetManager;

	static Asset* LoadLevel(const std::string& aPath);
};

