#include "ShaderStructs.hlsli"


VertexToPixel vertexShader(VertexInput input)
{
	VertexToPixel returnValue;

#if HAS_BONES
	float4 skinnedPosition = float4(0, 0, 0, 0);
	for (uint i = 0; i < BONESPERVERTEX; i++)
	{
		skinnedPosition += mul(input.myPosition, AnimationBuffer.myTransforms[i] * input.myBoneWeights[i];
	}
	input.myPosition = skinnedPosition;
#endif

	float4x4 packedPosition = {
		input.myPosition.x,input.myPosition.y,input.myPosition.z,input.myPosition.w,
		input.myNormal.x,input.myNormal.y,input.myNormal.z,0,
		input.myTangent.x,input.myTangent.y,input.myTangent.z,0,
		input.myBiTangent.x,input.myBiTangent.y,input.myBiTangent.z,0
	};

	float4x4 worldPosition = mul(packedPosition, ObjectBuffer.myModelToWorldSpace);
	float4x4 cameraPosition = mul(worldPosition, FrameBuffer.myWorldToCamera);
	float4x4 screenPosition = mul(cameraPosition, FrameBuffer.myCameraToProjection);


	returnValue.myPosition = worldPosition[0]; // input.myPosition; // -screenPosition[0];
	returnValue.myPosition.w = 1.0;
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
#if VERTEXCOLOR
	returnValue.myColor = input.myColor;
#endif

	return returnValue;
}