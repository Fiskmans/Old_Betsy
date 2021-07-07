#include "../../DeferredShaderStructs.hlsli"

struct TerrainVertexToPixel
{
	float4 myScreenPos		: SV_POSITION;
	float3 myWorldPos		: POSITION;
	float3 myNormal			: NORMAL;
	uint myPrimitiveIndex	: SV_PrimitiveId;
};

GBufferOutput pixelShader(TerrainVertexToPixel input)
{
	GBufferOutput output;
	output.myWorldPosition = float4(input.myWorldPos,1);
	output.myAlbedo.rgba = float4(
		lerp(0.7, 0.3, abs(cos(input.myPrimitiveIndex))),
		lerp(0.7, 0.3, abs(cos(input.myPrimitiveIndex*11))),
		lerp(0.7, 0.3, abs(cos(input.myPrimitiveIndex*53))),
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