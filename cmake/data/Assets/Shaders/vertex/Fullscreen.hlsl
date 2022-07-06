#include "../FullscreenShaderStructs.hlsli"

VertexToPixel vertexShader(VertexInput input)
{
	float4 positions[3] =
	{
		float4(-1,-1,0,1),
		float4(-1,3,0,1),
		float4(3,-1,0,1)
	};

	float2 uvs[3] =
	{
		float2(0,1),
		float2(0,-1),
		float2(2,1)
	};

	VertexToPixel returnValue;
	returnValue.myPosition = positions[input.myIndex];
	returnValue.myUV = uvs[input.myIndex];
	return returnValue;
}