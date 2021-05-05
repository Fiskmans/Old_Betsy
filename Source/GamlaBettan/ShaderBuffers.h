#pragma once
#include "Matrix4x4.hpp"
#include "Vector.hpp"


struct FrameBufferData
{
	CommonUtilities::Matrix4x4<float> myWorldToCamera;
	CommonUtilities::Matrix4x4<float> myCameraToProjection;
	V3F myEnvironmentLightDirection = { 1.f, 0.f, 0.f};
	float myShadowIntensity = 0.f;
	V3F myEnvironmentLightColor = { 1.f, 1.f, 1.f };
	float myEnviromentLightIntensity = 0.f; 
	V3F myCameraPosition = { 0.f, 0.f, 0.f };
	float myTotalTime = 0.f;
	CommonUtilities::Matrix4x4<float> myWorldToShadowCamera;
	CommonUtilities::Matrix4x4<float> myCameraToShadowProjection;
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
	CommonUtilities::Matrix4x4<float> myModelToWorldSpace;
	V4F myTint = { 0, 0, 0, 1 };
	unsigned int myBoneOffsetIndex = 0;
	unsigned int myNumOfUsedPointLights = 0;
	float myObjectLifeTime = 0;
	float myObjectExpectedLifeTime = 0;
	float myTimeSinceLastInteraction = -1.f;
	int myIsEventActive;

	unsigned int myObjectId;
	float trash;
};