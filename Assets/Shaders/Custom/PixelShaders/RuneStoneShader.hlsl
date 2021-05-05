#include "Data/Shaders/ShaderStructs.hlsli"


PixelOutput pixelShader(VertexToPixel input)
{
#include "Data/Shaders/Sampling.hlsli"

	//Calculate the angle value for every pixel
	float intensity = dot(normalize(environmentLightDirection.xyz), normalize(input.myNormal.xyz));
	if (intensity < 0)
		intensity = 0;
	float4 color = AlbedoMap.Sample(defaultSampler, input.myUV).rgba * float4(environmentLightColor, 1) * enviromentLightIntensity;

	if (intensity > 0)
		color = float4(1.0, 1.0, 1.0, color.a) * color;

	else
		color = float4(0.9, 0.9, 0.9, color.a) * color;

	/*
	float4 color = AlbedoMap.Sample(defaultSampler, input.myUV).rgba * float4(environmentLightColor, 1) * enviromentLightIntensity;
	float4 material = MaterialMap.Sample(defaultSampler, input.myUV).rgba;
	float4 normalTexture = NormalMap.Sample(defaultSampler, float2(input.myUV1.x, clamp(input.myUV1.y + objectLifeTime * 0.1, 0, 0.8))).rgba;

	float4 runeColorChange = material.r * normalTexture;

	float runeShineR = float(material.r * objectLifeTime) * 0.1;
	float runeShineG = float(material.g * objectLifeTime) * 0.25;
	float runeShineB = float(material.b * objectLifeTime) * 0.1;
	//Default output will add dot product of light direction	

	PixelOutput returnValue;
	returnValue.myColor = color + float4(0, runeShineR + runeShineG + runeShineB * 3,0,1);
	returnValue.myColor += runeColorChange;
	*/

	float minMask = 0.33;
	float maxMask = 0.6;

	float runeMaskA = lerp(color.a * minMask, color.a * maxMask, sin(objectLifeTime));

	PixelOutput returnValue;
	returnValue.myColor = color + float4(runeMaskA*0.2, runeMaskA*0.1, runeMaskA*0.6, 1);



	return returnValue;
}