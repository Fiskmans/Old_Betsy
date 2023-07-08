struct FrameBufferType
{
	float4x4 myWorldToCamera;
	float4x4 myCameraToProjection;

	float myTotalTime;
	float myPadding[3];
};

cbuffer FrameBuffer : register(FRAME_BUFFER)
{
	FrameBufferType FrameBuffer;
};

struct ObjectBufferType
{
	float4x4 myModelToWorldSpace;
	float4 myDiffuseColor;

	float myObjectLifeTime;
	uint myObjectId;

	float myPadding[2];
};

cbuffer ObjectBuffer : register(OBJECT_BUFFER)
{
	ObjectBufferType ObjectBuffer;
};

struct PointLight
{
	float3 myPosition;
	float myIntensity;

	float3 mycolor;
	float myRange;
};

struct PointLightBufferType
{
	PointLight myPointLights[NUMBEROFPOINTLIGHTS];

	uint myNumOfUsedPointLights;
	float myPadding[3];
};

cbuffer PointLightBuffer : register(POINT_LIGHT_BUFFER)
{
	PointLightBufferType PointLightBuffer;
}

struct AnimationBufferType
{
	float4x4 myTransforms[NUMBEROFANIMATIONBONES];
};

cbuffer AnimationBuffer : register(ANIMATION_BUFFER)
{
	AnimationBufferType AnimationBuffer;
}

struct DeferredFrameBufferType
{
	float3 cameraPosition;
	float enviromentLightIntensity;
	float3 environmentLightColor;
	float time;
	float3 environmentLightDirection;
	float cloudIntensity;
	float4x4 myToCamera;
	float4x4 myCameraProjection;
	float3 myCameraDirection;
	float trash_2;
};

cbuffer DeferredFrameBuffer : register(DEFERRED_FRAME_BUFFER)
{
	DeferredFrameBufferType DeferredFrameBuffer;
};