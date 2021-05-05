#include "ShaderStructs.hlsli"
#include "PBRFunctions.hlsli"


PixelOutput pixelShader(VertexToPixel input)
{
#include "Sampling.hlsli"

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

	returnValue.myColor.rgb = pow(abs(radiance), 1.0 / 2.2);
	returnValue.myColor.a = tint.a * albedoData.a;

	return returnValue;
}