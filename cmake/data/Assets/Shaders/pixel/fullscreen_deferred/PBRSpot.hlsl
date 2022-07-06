#include "../../FullscreenShaderStructs.hlsli"
#include "../../PBRFunctions.hlsli"

cbuffer LightBuffer : register(b0)
{
	float4x4 toCamera;
	float4x4 toProj;

	float3 Position;
	float Intensity;
	float3 CameraPosition;
	float Range;
};

TextureCube EnvironmentLight : register(t8);

Texture2D Depth : register(t15);

PixelOutput pixelShader(VertexToPixel input)
{
	float bias = 0.001;
	if (Depth.Sample(defaultSampler, input.myUV).r == 1)
	{
		discard;
	}

	PixelOutput returnValue;
	float3 worldPosition = resource1.Sample(defaultSampler, input.myUV).rgb;
	float3 albedo = resource2.Sample(defaultSampler, input.myUV).rgb;
	float3 normal = resource3.Sample(defaultSampler, input.myUV).rgb;
	float metalness = resource5.Sample(defaultSampler, input.myUV).r;
	float roughness = resource6.Sample(defaultSampler, input.myUV).r;

	float3 toEye = normalize(CameraPosition - worldPosition);

	float3 specColor = lerp((float3) 0.04, albedo, metalness);
	float3 diffColor = lerp((float3) 0.00, albedo, 1 - metalness);

	


	float3 toLight = worldPosition - Position;
	 
	float4 camPos = mul(toCamera, float4(worldPosition, 1));
	float4 projPos = mul(toProj,camPos);
	projPos /= projPos.w;
	if (projPos.x < -1 || projPos.x > 1 || projPos.y < -1 || projPos.y > 1 || projPos.z < -1 || projPos.z > 1)
	{
		discard;
	}
	float2 projuv = (projPos.xy + float2(1, 1)) * float2(0.5, -0.5);
	

	float4 spotData = resource10.Sample(defaultSampler, projuv);
	float depth = resource9.Sample(defaultSampler, projuv * float2(1/6.0,1)).r;

	if (depth < projPos.z - bias)
	{
		discard;
	}
	
    returnValue.myColor.rgba = pow(abs(EvaluatePointLight_Alpha(diffColor, specColor, normal, roughness, spotData.rgb, Intensity * 100 * pow(spotData.a, 8) * (1-projPos.z), Range, Position, worldPosition, toEye)), 1.0 / 2.2);
    returnValue.myColor.a = lerp(0, returnValue.myColor.a, saturate(projPos.z*1));
	
	return returnValue;
}