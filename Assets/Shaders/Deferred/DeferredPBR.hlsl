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

	float3 specColor = lerp((float3) 0.04, albedo, metalness);
	float3 diffColor = lerp((float3) 0.00, albedo, 1 - metalness);


	float3 ambience = EvaluateAmbience(EnvironmentLight, normal, worldNormal, toEye, roughness, metalness, albedo, ambientOcclusion, diffColor, specColor);
	float3 directionalLight = EvaluateDirectionalLight(diffColor, specColor, normal, roughness, enviromentLightIntensity, environmentLightColor.rgb, environmentLightDirection.xyz, toEye);


	float3 emColor = albedo * emissive;
	float3 radiance = ambience + directionalLight + emColor;

	returnValue.myColor.rgb = pow(abs(radiance), 1.0 / 2.2);
	returnValue.myColor.a = 1;

	return returnValue;

	return returnValue;
}