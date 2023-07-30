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


	float4 worldPosition = mul(input.myPosition, ObjectBuffer.myModelToWorldSpace);
	float4 cameraPosition = mul(worldPosition, FrameBuffer.myWorldToCamera);
	float4 screenPosition = mul(cameraPosition, FrameBuffer.myCameraToProjection);

	returnValue.myPosition = worldPosition;
	returnValue.myPosition.w = 1;
	returnValue.myNormal = float4(1.0, 0.0, 0.0, 0.0);
	returnValue.myTangent = float4(0.0, 1.0, 0.0, 0.0);
	returnValue.myBiTangent = float4(0.0, 0.0, 1.0, 0.0);

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