#include "Data/Shaders/ShaderStructs.hlsli"

struct cutomInput
{
	unsigned int vID : SV_VERTEXID;
};

const static float4 positions[6] =
{
	float4(700,188,700,1),
	float4(700,188,-700,1),
	float4(-700,188,-700,1),

	float4(700,188,700,1),
	float4(-700,188,-700,1),
	float4(-700,188,700,1)
};
const static float2 uvs[6] =
{
	float2(0,0),
	float2(0,1),
	float2(1,1),

	float2(0,0),
	float2(1,1),
	float2(1,0)
};

VertexToPixel vertexShader(cutomInput input)
{
	VertexToPixel returnValue;

	float4 origpos = positions[input.vID];

	float4x4 packedPosition = {
		origpos.x,origpos.y,origpos.z,origpos.w,
		0,1,0,0,
		1,0,0,0,
		0,0,1,0
	};


	float4x4 worldPosition = mul(packedPosition, modelToWorldMatrix);
	float4x4 cameraPosition = mul(worldPosition, worldToCameraMatrix);
	float4x4 screenPosition = mul(cameraPosition, cameraToProjectionMatrix);


	returnValue.myPosition = screenPosition[0];
	returnValue.myNormal = normalize(worldPosition[1]);
	returnValue.myTangent = worldPosition[2];
	returnValue.myBiTangent = worldPosition[3];
	returnValue.myUV = uvs[input.vID];

#ifdef MULTIPLE_UV
#error
#endif

	returnValue.myWorldPos = worldPosition[0];
#ifdef VERTEXCOLOR
#error
#endif
	return returnValue;
}