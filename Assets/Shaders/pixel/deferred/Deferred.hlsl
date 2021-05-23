#include "../../DeferredShaderStructs.hlsli"

GBufferOutput pixelShader(VertexToPixel input)
{
#include "../../Sampling.hlsli"
	GBufferOutput output;
	output.myWorldPosition = input.myWorldPos;
	output.myAlbedo.rgba = float4(albedo, 1);
	//dont think about it
	output.myNormal = float4(pixelNormal, 1);
	output.myVertexNormal = float4(input.myNormal.xyz,1);
	output.myMetalness = metalness;
	output.myRoughness = roughness;
	output.myAmbientOcclusion = ambientOcclusion;
	output.myEmissive = emissive;
	return output;

}