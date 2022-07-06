#include "../SpriteShaderStructs.hlsli"


const static float4 positions[6] =
{
	float4(0,0,0,1),
	float4(1,-1,0,1),
	float4(0,-1,0,1),

	float4(0,0,0,1),
	float4(1,0,0,1),
	float4(1,-1,0,1)
};
const static float2 uvs[6] =
{
	float2(0,0),
	float2(1,1),
	float2(0,1),

	float2(0,0),
	float2(1,0),
	float2(1,1)
};

VertexToPixel vertexShader(VertexInput input)
{
    VertexToPixel returnValue;


	float4 pos = positions[input.myIndex];

	float4 pivotPos = mul(PivotToModel, pos);

    returnValue.myPosition = mul(ModelToWorld, pivotPos);
	returnValue.myPosition.z = depth;
	float2 uv = uvs[input.myIndex];

	returnValue.myUV.x = (uv.x + UVMin.x) * (UVMax.x - UVMin.x);
	returnValue.myUV.y = (uv.y + UVMin.y) * (UVMax.y - UVMin.y);

    return returnValue;
}