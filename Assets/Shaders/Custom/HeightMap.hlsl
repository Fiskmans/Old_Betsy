#include "Data/Shaders/ShaderStructs.hlsli"

SamplerState defaultSampler : register(s0);

VertexToPixel vertexShader(VertexInput input)
{
	VertexToPixel returnValue;

#ifdef HAS_BONES
	float4 skinnedPosition = float4(0,0,0,0);
	float4 color = float4(input.myBoneWeights[0], input.myBoneWeights[1], input.myBoneWeights[2], input.myBoneWeights[3]);
	for (uint i = 0; i < BONESPERVERTEX; i++)
	{
		skinnedPosition += mul(input.myPosition, myBoneTransforms[input.myBones[i]]) * input.myBoneWeights[i];
	}
	input.myColor = color;
	input.myPosition = skinnedPosition;
#endif

	input.myPosition += input.myNormal * MaterialMap.SampleLevel(defaultSampler, input.myUV,0).r * 10;


	float4x4 packedPosition = {
		input.myPosition.x,input.myPosition.y,input.myPosition.z,input.myPosition.w,
		input.myNormal.x,input.myNormal.y,input.myNormal.z,0,
		input.myTangent.x,input.myTangent.y,input.myTangent.z,0,
		input.myBiTangent.x,-input.myBiTangent.y,input.myBiTangent.z,0
	};


	float4x4 worldPosition = mul(packedPosition, modelToWorldMatrix);
	float4x4 cameraPosition = mul(worldPosition, worldToCameraMatrix);
	float4x4 screenPosition = mul(cameraPosition, cameraToProjectionMatrix);


	returnValue.myPosition = screenPosition[0];
	returnValue.myNormal = normalize(worldPosition[1]);
	returnValue.myTangent = worldPosition[2];
	returnValue.myBiTangent = worldPosition[3];
	returnValue.myUV = input.myUV;

#ifdef MULTIPLE_UV
	returnValue.myUV1 = input.myUV1;
	returnValue.myUV2 = input.myUV2;
#endif

	returnValue.myWorldPos = worldPosition[0];
#ifdef VERTEXCOLOR
	returnValue.myColor = input.myColor;
#endif

	return returnValue;
}