#pragma once

struct PointLightInstanceBuffer
{
	int id = 0;
	float position[3] = { 0, 0, 0 };
	float color[3] = { 0, 0, 0 };
	float intensity = 0;
	float period = 0;
	float minVal = 0;
	float range = 0;
};

struct StaticObjectInstanceBuffer
{
	std::string aFilePath = "";
	float position[3] = { 0, 0, 0 };
	float rotation[3] = { 0, 0, 0 };
	float scale[3] = { 1.0f, 1.0f, 1.0f };
	float bbmin[3] = { 0, 0, 0 };
	float bbmax[3] = { 0, 0, 0 };
};

struct DynamicObjectInstanceBuffer
{
	std::string aFilePath = "";
	float position[3] = { 0, 0, 0 };
	float rotation[3] = { 0, 0, 0 };
	float scale[3] = { 1.0f, 1.0f, 1.0f };
	float bbmin[3] = { 0, 0, 0 };
	float bbmax[3] = { 0, 0, 0 };
};

struct InteractableInstanceBuffer
{
	std::string aFilePath = "";
	float position[3] = { 0, 0, 0 };
	float rotation[3] = { 0, 0, 0 };
	float scale[3] = { 1.0f, 1.0f, 1.0f };
	float interactPos[3] = { 0.0f, 0.0f, 0.0f };
	int interactID = 0;
	unsigned int ID;
	std::string aImagePath = "";
};

struct DestructibleObjectInstanceBuffer
{
	std::string aFilePath = "";
	float position[3] = { 0, 0, 0 };
	float rotation[3] = { 0, 0, 0 };
	float scale[3] = { 1.0f, 1.0f, 1.0f };
	int type = 0;
};

struct EnemyInstanceBuffer
{
	std::string aFilePath = "";
	int enemyID = 0;
	int animationID = 0;
	int triggerVolumeID = -1;
	int targetPosID = -1;
	float position[3] = { 0, 0, 0 };
	float rotation[3] = { 0, 0, 0 };
	float scale[3] = { 1.0f, 1.0f, 1.0f };
};

struct TriggerBoxInstanceBuffer
{
	std::string aFilePath = "";
	float ID = 0;
	float targetID = 0;
	float position[3] = { 0, 0, 0 };
	float rotation[3] = { 0, 0, 0 };
	float scale[3] = { 0, 0, 0 };
	bool isAbilityTriggered = false;
};

struct DirLightInstanceBuffer
{
	float direction[3] = { 0, 0, 0 };
	float color[3] = { 0, 0, 0 };
	float intensity = 0;
};

struct PhysXBoxInstanceBuffer
{
	std::string aFilePath = "";
	float position[3] = { 0, 0, 0 };
	float rotation[3] = { 0, 0, 0 };
	float scale[3] = { 1.0f, 1.0f, 1.0f };
};

struct SpotLightBuffer
{
	std::string aFilePath;
	V3F aTranslation;
	float aFov;
	float aRange;
	float aIntensity;
	V3F aRotation;
};

struct DecalBuffer
{
	std::string aFilePath;
	V3F aTranslation;
	V3F aRotation;
};