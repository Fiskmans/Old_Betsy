#include "../Fullscreen/FullscreenShaderStructs.hlsli"
#include "../PBRFunctions.hlsli"

cbuffer framebuffer : register(b0)
{
	float3 cameraPosition;
	float enviromentLightIntensity;
	float3 environmentLightColor;
	float trash;
	float3 environmentLightDirection;
	float trash_1;
};

TextureCube EnvironmentLight : register(t8);


float3 getPositionBack(in float2 uv) 
{ 
	return resource11.Sample(defaultSampler, uv).xyz; 
}

float3 getPosition(in float2 uv)
{
	return resource1.Sample(defaultSampler, uv).xyz;
}

float doAmbientOcclusion(in float2 tcoord, in float2 uv, in float3 p, in float3 cnorm)
{
	float g_bias = 0.0;
	float g_scale = 0.001;
	float3 diff = getPosition(tcoord + uv) - p;
	const float3 v = normalize(diff);
	const float d = length(diff) * g_scale;
	return max(0.0, dot(cnorm, v) - g_bias) * (1.0 / (1.0 + d)) * 1;
}

float doAmbientOcclusionBack(in float2 tcoord, in float2 uv, in float3 p, in float3 cnorm)
{
	float g_bias = 0.0;
	float2 suv =  (tcoord + uv);
	float3 diff = getPositionBack(suv) - p;
	const float3 v = normalize(diff);
	const float d = length(diff) * 0.0001;
	return max(0.0, dot(cnorm, v) - g_bias)* (1.0 / (1.0 + d));
}

float2 getRandom(in float2 uv)
{
	float2 suv = GetTextureSize(resource1).xy * uv / 640;
	float2 sampl = resource10.Sample(defaultSampler, suv).xy;
	float2 scaled = sampl * float2(2.0f, 2.0f) - float2(1.0f, 1.0f);
	float2 normlz = normalize(scaled);
	return normlz;
}

PixelOutput pixelShader(VertexToPixel input)
{

	PixelOutput returnValue;
	float3 worldPosition = resource1.Sample(defaultSampler, input.myUV).rgb;
	float3 albedo = resource2.Sample(defaultSampler, input.myUV).rgb;
	float3 normal = resource3.Sample(defaultSampler, input.myUV).rgb;
	float3 worldNormal = resource4.Sample(defaultSampler, input.myUV).rgb;
	float metalness = resource5.Sample(defaultSampler, input.myUV).r;
	float roughness = resource6.Sample(defaultSampler, input.myUV).r;
	float ambientOcclusion = resource7.Sample(defaultSampler, input.myUV).r;
	float emissive = resource8.Sample(defaultSampler, input.myUV).r;
	float3 toEye = normalize(cameraPosition.xyz - worldPosition);


	const float2 vec[4] = { float2(1,0),float2(-1,0), float2(0,1),float2(0,-1) };
	float2 rand = getRandom(worldPosition.xy);

	float3 toCamera = cameraPosition - worldPosition;

	float depth = length(toCamera) / 10000;

	float ao = 0;
	float rad = 0.04 * depth;
	returnValue.myColor.r = 0;
	int iterations = 4;// lerp(4.0, 2.0, depth);
	[unroll] for (int j = 0; j < iterations; ++j)
	{
		float2 coord1 = reflect(vec[j], rand) * rad;
		float2 coord2 = float2(coord1.x * 0.707 - coord1.y * 0.707, coord1.x * 0.707 + coord1.y * 0.707);
		returnValue.myColor.r = input.myUV.x+coord1.x*10;

		ao += doAmbientOcclusion(input.myUV, coord1 * 0.25, worldPosition, normal);
		ao += doAmbientOcclusion(input.myUV, coord2 * 0.5, worldPosition, normal);
		ao += doAmbientOcclusion(input.myUV, coord1 * 0.75, worldPosition, normal);
		ao += doAmbientOcclusion(input.myUV, coord2, worldPosition, normal);
	}

	ao /= (float)iterations * 4.0;
	float powval = 9;
	float b_ao = pow(abs(ambientOcclusion), powval);
	float ss_ao = pow(abs(1 - ao), powval);

	returnValue.myColor.r =b_ao*ss_ao;
	//returnValue.myColor.r = normal.x;// (ao);// *ambientOcclusion;
	returnValue.myColor.gba = float3(0, 0, 0);

	return returnValue;
}