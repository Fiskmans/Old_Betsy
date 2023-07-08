#include "ShaderStructs.hlsli"
#include "Sampling.hlsli"

GBufferOutput pixelShader(VertexToPixel input)
{
	SampleOutput sampleData = Sample(input);

	GBufferOutput output;
	output.myWorldPosition = input.myWorldPos;
	output.myAlbedo.rgba = float4(sampleData.albedo, 1);
	//dont think about it
	output.myNormal = float4(sampleData.pixelNormal, 1);
	output.myVertexNormal = float4(input.myNormal.xyz,1);
	output.myMaterial.r = sampleData.metalness;
	output.myMaterial.g = sampleData.roughness;
	output.myMaterial.b = sampleData.ambientOcclusion;
	output.myMaterial.a = sampleData.emissive;
	return output;
}