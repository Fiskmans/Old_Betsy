

struct VertexInput
{
	float4 myPosition	:	POSITION;
	float4 myMovement	:	MOVEMENT;
	float4 myColor		:	COLOR;
	float2 mySize		:	SIZE;
	float myDistToCam	:	DISTANCE;
	float myLifeTime	:	LIFETIME;
    float2 myUVMin      :   UVMIN;
    float2 myUVMax      :   UVMAX;
    float myFBTimer     :   FBTIMER;
};

struct VertexToGeometry
{
	float4 myPosition	:	POSITION;
	float4 myColor		:	COLOR;
	float2 mySize		:	SIZE;
    float2 myUVMin      :   UVMIN;
    float2 myUVMax      :   UVMAX;
    //float myFBTimer     :   FBTIMER;
};

struct GeometryToPixel
{
	float4 myPosition	:	SV_POSITION;
	float4 myColor		:	COLOR;
	float2 myUV			:	UV;
};

struct PixelOutput
{
	float4 myColor		:	SV_TARGET;
};


cbuffer frameData : register(b0)
{
	float4x4 worldToCameraMatrix;
	float4x4 cameraToProjectionMatrix;
	float3 cameraForward;
	float totalTime;
};

cbuffer objectData : register(b1)
{
	float4x4 particleToWorldMatrix;
};


Texture2D Texture						: register(t0);
SamplerState DefaultSampler				: register(s0);