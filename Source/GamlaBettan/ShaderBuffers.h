#pragma once

struct FrameBufferData
{
	M44f myWorldToCamera;
	M44f myCameraToProjection;
	V3F myEnvironmentLightDirection = { 1.f, 0.f, 0.f};
	float myShadowIntensity = 0.f;
	V3F myEnvironmentLightColor = { 1.f, 1.f, 1.f };
	float myEnviromentLightIntensity = 0.f; 
	V3F myCameraPosition = { 0.f, 0.f, 0.f };
	float myTotalTime = 0.f;
	M44f myWorldToShadowCamera;
	M44f myCameraToShadowProjection;
	V3F myCameraDirection = { 0.f,0.f,0.f };
	float trash_0;
};

struct ObjectBufferData
{
	struct PointLight
	{
		V3F position = { 0, 0, 0 };
		float intensity = 0.f;
		V3F color = { 1, 1, 1 };
		float range = 0.f;
	} myPointLights[NUMBEROFPOINTLIGHTS];

	M44f myModelToWorldSpace;
	V4F myTint = { 0, 0, 0, 1 };
	unsigned int myBoneOffsetIndex = 0;
	unsigned int myNumOfUsedPointLights = 0;
	float myObjectLifeTime = 0;
	unsigned int myObjectId;

	V3F myDiffuseColor = { 0.5,0.5,0.5 };
	float trash = 0.5f;
};