#include "Data/Shaders/ShaderStructs.hlsli"
SamplerState defaultSampler : register(s0);
#include "Data/Shaders/PBRFunctions.hlsli"

SamplerState MeshTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


PixelOutput pixelShader(VertexToPixel input)
{
#include "Data/Shaders/Sampling.hlsli"

	PixelOutput returnValue;

	float3 toEye = normalize(cameraPosition.xyz - input.myWorldPos.xyz);

	float3 specColor = lerp((float3) 0.04, albedo, metalness);
	float3 diffColor = lerp((float3) 0.00, albedo, 1 - metalness);

	float3 ambience = EvaluateAmbience(EnvironmentLight, pixelNormal, input.myNormal.xyz, toEye, roughness, metalness, albedo, ambientOcclusion, diffColor, specColor);
	float3 directionalLight = EvaluateDirectionalLight(diffColor, specColor, pixelNormal, roughness, enviromentLightIntensity, environmentLightColor.rgb, environmentLightDirection.xyz, toEye);

	float3 pointLightsLight = 0;
	for (unsigned int i = 0; i < numOfUsedPointLights; i++)
	{
		PointLight currLight = pointLights[i];
		pointLightsLight += EvaluatePointLight(diffColor, specColor, pixelNormal, roughness, currLight.color.rgb, currLight.intensity, currLight.range, currLight.position, input.myWorldPos.xyz, toEye);
	}

	ambience = ambience * 0.2f;
	float3 emColor = albedo * emissive;
	float3 radiance = ambience + directionalLight + emColor + pointLightsLight;


	//New hlsl

	float redChannelMask = MaterialMap.Sample(MeshTextureSampler, input.myUV2).r;
	float redChannelColor = redChannelMask * 2;
	

	float onInputTriggerTime = totalTime - objectLifeTime;
	redChannelColor = lerp(redChannelColor, redChannelColor *10, 1.2 - (cos((objectLifeTime) * 6) + 1) / 2);


	float3 EmissiveRGB = float3(redChannelColor, 0, redChannelColor/5) * 1.5;
	//


	returnValue.myColor.rgb = pow(abs(radiance), 1.0 / 2.2) + EmissiveRGB;
	returnValue.myColor.a = tint.a * albedoData.a;// +(redMask * 10);


	return returnValue;
}