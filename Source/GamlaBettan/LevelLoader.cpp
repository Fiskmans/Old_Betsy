#include "pch.h"
#include "LevelLoader.h"
#include "Asset.h"

bool ParseSkybox(std::istream& aStream, LevelParseResult& aResult)
{
	std::getline(aStream, aResult.mySkyboxPath);
	if (!aStream)
	{
		SYSERROR("Loading level failed at skybox setup", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseWorldAxis(std::istream& aStream, LevelParseResult& aResult)
{
	aStream >> aResult.myWorldAxisSize;
	if (!aStream)
	{
		SYSERROR("Loading level failed at world axis", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseStaticObjects(std::istream& aStream, LevelParseResult& aResult)
{

	int modelCount = 0;
	aStream >> modelCount;

	StaticObjectInstanceBuffer buffer;
	for (size_t i = 0; i < modelCount; i++)
	{
		if (aStream >> buffer.aFilePath
			>> buffer.position[0] >> buffer.position[1] >> buffer.position[2]
			>> buffer.rotation[0] >> buffer.rotation[1] >> buffer.rotation[2]
			>> buffer.scale[0] >> buffer.scale[1] >> buffer.scale[2])
		{
			aResult.myStaticObjects.push_back(buffer);
		}
	}

	if (!aStream)
	{
		SYSERROR("Static objects broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseInteractables(std::istream& aStream, LevelParseResult& aResult)
{
	int modelCount = 0;
	aStream >> modelCount;

	InteractableInstanceBuffer buffer;
	for (size_t i = 0; i < modelCount; i++)
	{
		if (aStream >> buffer.aFilePath >> buffer.interactID
			>> buffer.position[0] >> buffer.position[1] >> buffer.position[2]
			>> buffer.rotation[0] >> buffer.rotation[1] >> buffer.rotation[2]
			>> buffer.scale[0] >> buffer.scale[1] >> buffer.scale[2]
			>> buffer.interactPos[0] >> buffer.interactPos[1] >> buffer.interactPos[2] >> buffer.aImagePath)
		{
			aResult.myInteractables.push_back(buffer);
		}
	}

	if (!aStream)
	{
		SYSERROR("Pickup objects broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseDecals(std::istream& aStream, LevelParseResult& aResult)
{
	int decalCount = 0;
	aStream >> decalCount;

	DecalBuffer buffer;
	for (size_t i = 0; i < decalCount; i++)
	{
		if (aStream >> buffer.aFilePath
			>> buffer.aTranslation.x >> buffer.aTranslation.y >> buffer.aTranslation.z
			>> buffer.aRotation.x >> buffer.aRotation.y >> buffer.aRotation.z)
		{
			aResult.myDecals.push_back(buffer);
		}
	}

	if (!aStream)
	{
		SYSERROR("Pickup objects broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseEnemies(std::istream& aStream, LevelParseResult& aResult)
{

	int enemyCount = 0;
	aStream >> enemyCount;

	EnemyInstanceBuffer enemyBuffer;

	for (size_t i = 0; i < enemyCount; i++)
	{
		if (aStream >> enemyBuffer.aFilePath
			>> enemyBuffer.enemyID
			>> enemyBuffer.animationID >> enemyBuffer.triggerVolumeID >> enemyBuffer.targetPosID
			>> enemyBuffer.position[0] >> enemyBuffer.position[1] >> enemyBuffer.position[2]
			>> enemyBuffer.rotation[0] >> enemyBuffer.rotation[1] >> enemyBuffer.rotation[2]
			>> enemyBuffer.scale[0] >> enemyBuffer.scale[1] >> enemyBuffer.scale[2])
		{
			aResult.myEnemyObjects.push_back(enemyBuffer);
		}
	}

	if (!aStream)
	{
		SYSERROR("Enemies broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseTriggers(std::istream& aStream, LevelParseResult& aResult)
{
	int triggerCount = 0;
	aStream >> triggerCount;

	TriggerBoxInstanceBuffer triggerBuffer;

	for (size_t i = 0; i < triggerCount; i++)
	{
		if (aStream >> triggerBuffer.aFilePath
			>> triggerBuffer.ID
			>> triggerBuffer.targetID
			>> triggerBuffer.position[0] >> triggerBuffer.position[1] >> triggerBuffer.position[2]
			>> triggerBuffer.rotation[0] >> triggerBuffer.rotation[1] >> triggerBuffer.rotation[2]
			>> triggerBuffer.scale[0] >> triggerBuffer.scale[1] >> triggerBuffer.scale[2] >> triggerBuffer.isAbilityTriggered)
		{
			aResult.myTriggerBuffer.push_back(triggerBuffer);
		}
	}

	if (!aStream)
	{
		SYSERROR("Triggers broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParsePoints(std::istream& aStream, LevelParseResult& aResult)
{
	//Positions
	int posCount = 0;
	aStream >> posCount;
	V3F pos;
	int ID;

	for (size_t i = 0; i < posCount; i++)
	{
		if (aStream >> ID >> pos.x >> pos.y >> pos.z)
		{
			aResult.myMayaPositions.emplace_back(ID, pos);
		}
	}

	if (!aStream)
	{
		SYSERROR("Maya positions broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseEnvironmentLight(std::istream& aStream, LevelParseResult& aResult)
{
	DirLightInstanceBuffer dirLightBuffer;
	dirLightBuffer.color[0] = 1.f;
	dirLightBuffer.color[1] = 1.f;
	dirLightBuffer.color[2] = 1.f;

	dirLightBuffer.direction[0] = 0.3f;
	dirLightBuffer.direction[1] = 0.5f;
	dirLightBuffer.direction[2] = 0.1f;
	dirLightBuffer.intensity = 1.f;

	std::string dirLight = "";
	int bob;
	aStream >> bob;
	aStream >> dirLight;

	if (dirLight == "DirectionalLight" && aStream
		>> dirLightBuffer.direction[0] >> dirLightBuffer.direction[1] >> dirLightBuffer.direction[2]
		>> dirLightBuffer.color[0] >> dirLightBuffer.color[1] >> dirLightBuffer.color[2] >> dirLightBuffer.intensity)
	{
		aResult.myEnvironmentlight = dirLightBuffer;
	}
	if (!aStream)
	{
		SYSERROR("Environmentlight  broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParsePointLights(std::istream& aStream, LevelParseResult& aResult)
{
	int lightCount = 0;
	aStream >> lightCount;
	std::string bob;

	PointLightInstanceBuffer lightBuffer;

	for (size_t i = 0; i < lightCount; i++)
	{
		if (aStream >> bob >> lightBuffer.id
			>> lightBuffer.position[0] >> lightBuffer.position[1] >> lightBuffer.position[2]
			>> lightBuffer.intensity >> lightBuffer.period >> lightBuffer.minVal >> lightBuffer.color[0]
			>> lightBuffer.color[1] >> lightBuffer.color[2] >> lightBuffer.range)
		{
			aResult.myPointLights.push_back(lightBuffer);
		}
	}

	if (!aStream)
	{
		SYSERROR("point lights broke level loader", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParsePlayerPos(std::istream& aStream, LevelParseResult& aResult)
{
	int dummyInt;
	int dummyInt2;
	if (!(aStream >> dummyInt >> dummyInt2 >> aResult.myPlayerStart.x >> aResult.myPlayerStart.y >> aResult.myPlayerStart.z))
	{
		SYSERROR("Player spawn position broke level loading", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseNavMesh(std::istream& aStream, LevelParseResult& aResult)
{
	int dummy;
	if (!(aStream >> dummy >> aResult.myNavMeshPath))
	{
		SYSERROR("navMesh broke level loading", aResult.myLevelFile);
		return false;
	}
	return true;
}

bool ParseCollisionBoxes(std::istream& aStream, LevelParseResult& aResult)
{
	int boxCount = 0;
	aStream >> boxCount;

	PhysXBoxInstanceBuffer physXBuffer;

	for (size_t i = 0; i < boxCount; i++)
	{
		if (aStream >> physXBuffer.aFilePath
			>> physXBuffer.position[0] >> physXBuffer.position[1] >> physXBuffer.position[2]
			>> physXBuffer.rotation[0] >> physXBuffer.rotation[1] >> physXBuffer.rotation[2]
			>> physXBuffer.scale[0] >> physXBuffer.scale[1] >> physXBuffer.scale[2])
		{
			aResult.myCollsionBoxes.push_back(physXBuffer);
		}
	}
	if (!aStream)
	{
		SYSERROR("physX boxes broke level loader", aResult.myLevelFile);
		return false;
	}
}

bool ParseDestructables(std::istream& aStream, LevelParseResult& aResult)
{
	int destructiblecount = 0;
	aStream >> destructiblecount;

	DestructibleObjectInstanceBuffer destructibleBuffer;

	for (size_t i = 0; i < destructiblecount; i++)
	{
		if (aStream >> destructibleBuffer.aFilePath
			>> destructibleBuffer.position[0] >> destructibleBuffer.position[1] >> destructibleBuffer.position[2]
			>> destructibleBuffer.rotation[0] >> destructibleBuffer.rotation[1] >> destructibleBuffer.rotation[2]
			>> destructibleBuffer.scale[0] >> destructibleBuffer.scale[1] >> destructibleBuffer.scale[2] >> destructibleBuffer.type)
		{
			aResult.myDestrucables.push_back(destructibleBuffer);
		}
	}

	if (!aStream)
	{
		SYSERROR("destructibles boxes broke level loader", aResult.myLevelFile);
		return false;
	}
}

bool ParseSpotlights(std::istream& aStream, LevelParseResult& aResult)
{
	int spotLightCount = 0;
	aStream >> spotLightCount;
	SpotLightBuffer spotBuffer;

	for (size_t i = 0; i < spotLightCount; i++)
	{
		if (aStream >> spotBuffer.aFilePath
			>> spotBuffer.aFov
			>> spotBuffer.aRange
			>> spotBuffer.aIntensity
			>> spotBuffer.aTranslation.x >> spotBuffer.aTranslation.y >> spotBuffer.aTranslation.z
			>> spotBuffer.aRotation.x >> spotBuffer.aRotation.y >> spotBuffer.aRotation.z)
		{
			aResult.mySpotlights.push_back(spotBuffer);
		}
	}
	if (!aStream)
	{
		SYSERROR("Spotlights broke level loader", aResult.myLevelFile);
		return false;
	}
}

bool ParseDynamicObjects(std::istream& aStream, LevelParseResult& aResult)
{
	int dynamicObjectCount = 0;
	aStream >> dynamicObjectCount;
	SpotLightBuffer spotBuffer;
	float _;

	for (size_t i = 0; i < dynamicObjectCount; i++)
	{
		if (aStream >> spotBuffer.aFilePath
			>> _ >> _ >> _
			>> _ >> _ >> _
			>> _ >> _ >> _)
		{
			//what is this even?
		}
	}
	if (!aStream)
	{
		SYSERROR("dynamicObjects broke level loader", aResult.myLevelFile);
		return false;
	}
}

LevelParseResult ParseLevelFile(const std::string& aFilePath)
{
	LevelParseResult result;
	result.myCounter = 0;
	result.myStep = 0;

	std::ifstream levelFile;
	levelFile.open(aFilePath);
	if (!levelFile.good())
	{
		SYSERROR("Could not open/find levelfile", aFilePath);
		return result;
	}

	result.myIsSuccessfull &= ParseSkybox(levelFile, result);
	result.myIsSuccessfull &= ParseWorldAxis(levelFile, result);
	result.myIsSuccessfull &= ParseStaticObjects(levelFile, result);
	result.myIsSuccessfull &= ParseEnemies(levelFile, result);
	result.myIsSuccessfull &= ParsePoints(levelFile, result);
	result.myIsSuccessfull &= ParseTriggers(levelFile, result);
	result.myIsSuccessfull &= ParseEnvironmentLight(levelFile, result);
	result.myIsSuccessfull &= ParsePointLights(levelFile, result);
	result.myIsSuccessfull &= ParsePlayerPos(levelFile, result);
	result.myIsSuccessfull &= ParseNavMesh(levelFile, result);
	result.myIsSuccessfull &= ParseCollisionBoxes(levelFile, result);
	result.myIsSuccessfull &= ParseDestructables(levelFile, result);
	result.myIsSuccessfull &= ParseSpotlights(levelFile, result);
	result.myIsSuccessfull &= ParseDecals(levelFile, result);
	result.myIsSuccessfull &= ParseDynamicObjects(levelFile, result);
	result.myIsSuccessfull &= ParseInteractables(levelFile, result);

	return result;
}

Asset* LevelLoader::LoadLevel(const std::string& aPath)
{
    return new LevelAsset(std::move(std::async(&ParseLevelFile, aPath)));
}
