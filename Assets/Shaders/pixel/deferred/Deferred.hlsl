#include "../../DeferredShaderStructs.hlsli"
#include "../../Sampling.hlsli"

GBufferOutput pixelShader(VertexToPixel input)
{
	SampleOutput sampleData = Sample(input);


	GBufferOutput output;
	output.myWorldPosition = input.myWorldPos;
	output.myAlbedo.rgba = float4(sampleData.albedo, 1);
	//dont think about it
	output.myNormal = float4(sampleData.pixelNormal, 1);
	output.myVertexNormal = float4(input.myNormal.xyz,1);
	output.myMetalness = sampleData.metalness;
	output.myRoughness = sampleData.roughness;
	output.myAmbientOcclusion = sampleData.ambientOcclusion;
	output.myEmissive = sampleData.emissive;
	return output;

}