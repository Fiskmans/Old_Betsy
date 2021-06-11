#include "../ShaderStructs.hlsli"

float4x4 ExtractBoneMatrix(uint aIndex)
{
	return transpose(float4x4(
		BoneTexture.Load(int3((aIndex * 4 + 0), myBoneOffsetIndex, 0)),
		BoneTexture.Load(int3((aIndex * 4 + 1), myBoneOffsetIndex, 0)),
		BoneTexture.Load(int3((aIndex * 4 + 2), myBoneOffsetIndex, 0)),
		BoneTexture.Load(int3((aIndex * 4 + 3), myBoneOffsetIndex, 0))
		));
}

VertexToPixel vertexShader(VertexInput input)
{
	VertexToPixel returnValue;

#if HAS_BONES
#if BONESPERVERTEX == 1
	input.myPosition = mul(input.myPosition, ExtractBoneMatrix(input.myBones[0])) * input.myBoneWeights[0];
#else
	float4 skinnedPosition = float4(0, 0, 0, 0);
	for (uint i = 0; i < BONESPERVERTEX; i++)
	{
		skinnedPosition += mul(input.myPosition, ExtractBoneMatrix(input.myBones[i])) * input.myBoneWeights[i];
	}
	input.myPosition = skinnedPosition;
#endif
#endif

	float4x4 packedPosition = {
		input.myPosition.x,input.myPosition.y,input.myPosition.z,input.myPosition.w,
		input.myNormal.x,input.myNormal.y,input.myNormal.z,0,
		input.myTangent.x,input.myTangent.y,input.myTangent.z,0,
		input.myBiTangent.x,input.myBiTangent.y,input.myBiTangent.z,0
	};

	float4x4 worldPosition = mul(packedPosition, modelToWorldMatrix);
	float4x4 cameraPosition = mul(worldPosition, worldToCameraMatrix);
	float4x4 screenPosition = mul(cameraPosition, cameraToProjectionMatrix);


	returnValue.myPosition = screenPosition[0];
	returnValue.myNormal = normalize(worldPosition[1]);
	returnValue.myTangent = worldPosition[2];
	returnValue.myBiTangent = worldPosition[3];

#if HAS_UV_SETS
	[unroll] for (uint i = 0; i < UV_SETS_COUNT; i++)
	{	
		returnValue.myUV[0] = input.myUV[0];
	}
#endif

	returnValue.myWorldPos = worldPosition[0];
#ifdef VERTEXCOLOR
	returnValue.myColor = input.myColor;
#endif

	return returnValue;
}