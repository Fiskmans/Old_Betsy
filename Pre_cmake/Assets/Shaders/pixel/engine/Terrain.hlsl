#include "../../DeferredShaderStructs.hlsli"

struct TerrainVertexToPixel
{
	float4 myScreenPos	: SV_POSITION;
	float3 myWorldPos	: POSITION;
	float3 myNormal		: NORMAL;
	uint   mySeed		: RANDOM_SEED;
};

float random(float val)
{
	return   (cos(val * 100 * 5 + 5.128)
			+ cos(val * 100 * 7 + 1.732)
			+ cos(val * 100 * 11 + 4.496)
			+ cos(val * 100 * 13 + 15.684) 
			+ cos(val * 100 * 17 + 3.693)
			+ cos(val * 100 * 19 + 8.827)
			+ cos(val * 100 * 23 + 18.495)) / 16.0 + 0.5;
}

GBufferOutput pixelShader(TerrainVertexToPixel input)
{
	GBufferOutput output;
	output.myWorldPosition = float4(input.myWorldPos,1);
	output.myAlbedo.rgba = float4(float3(208.0/255.0, 148.0/255.0,69.0/255.0) +
		float3(
			(random(input.mySeed) * 2.0 - 1.0) * 30.0 / 255.0,
			(random(input.mySeed + 1000) * 2.0 - 1.0) * 25.0 / 255.0,
			(random(input.mySeed + 2000) * 2.0 - 1.0) * 1.0/255.0
			),
		1.0);
	//dont think about it
	output.myNormal = float4(input.myNormal, 0);
	output.myVertexNormal = float4(input.myNormal, 0);
	output.myMetalness = 0;
	output.myRoughness = 0;
	output.myAmbientOcclusion = 0;
	output.myEmissive = 0;
	return output;
}