#include "Data/Shaders/ShaderStructs.hlsli"


PixelOutput pixelShader(VertexToPixel input)
{
#include "Data/Shaders/Sampling.hlsli"

	//Calculate the angle value for every pixel
	//float c = float(worldToCameraMatrix._m31);
	//float3 cameraDirection = float3(worldToCameraMatrix._m31, worldToCameraMatrix._m32, worldToCameraMatrix._m33);
	
	//Directional light fresnel
	float intensity = dot(normalize(environmentLightDirection.xyz), normalize(input.myNormal.xyz));
	if (intensity < 0)
		intensity = 0;

	float4 colorDirectionalLight = albedoData * float4(environmentLightColor, 1) * enviromentLightIntensity;

	if (intensity > 0)
		colorDirectionalLight = float4(1.0 * colorDirectionalLight.r, 1.0 * colorDirectionalLight.g, 1.0 * colorDirectionalLight.b, 1);

	else
		colorDirectionalLight = float4(0.1 * colorDirectionalLight.r, 0.1 * colorDirectionalLight.g, 0.1 * colorDirectionalLight.b, 1);
	

	//Camera direction fresnel
	float3 calculatedCameraDirection = normalize(cameraPosition - input.myWorldPos.xyz);
	float intensityCameraFresnel = dot(normalize(calculatedCameraDirection.xyz), normalize(input.myNormal.xyz));

	float4 colorCamera = albedoData * float4(environmentLightColor, 1) * enviromentLightIntensity;

	float innerGlowPulse = lerp(0.999, 0.988, sin(objectLifeTime) * sin(objectLifeTime));
	
	float middleShadowPulse = lerp(1.4, 1.15, sin(objectLifeTime) * sin(objectLifeTime));
	float middleGlowPulse = lerp(3, 2, sin(objectLifeTime) * sin(objectLifeTime));
	
	float outerGlowPulse = lerp(4, 2.5, sin(objectLifeTime) * sin(objectLifeTime));

	float innerGlow = 4;


	if (intensityCameraFresnel > 0.99 * innerGlowPulse)
		colorCamera = float4(1.0, 1.0 , 1.0 , 1);

	else if (intensityCameraFresnel > 0.7 * middleShadowPulse)
		colorCamera = float4(0.5, 0.5, 0.5,  0.1);

	else if (intensityCameraFresnel > 0.27 * middleGlowPulse)
		colorCamera = float4(0.75, 0.75, 0.75, 0.125);

	else if (intensityCameraFresnel > 0.18 * outerGlowPulse)
		colorCamera = float4(0.9, 0.9, 0.9, 0.025);

	else
		colorCamera = float4(0,0,0,0);

	float minMask = 3.1;
	float maxMask = 3.2;

	float colorLerp = lerp(colorCamera.a * minMask, colorCamera.a * maxMask, sin(objectLifeTime));
	colorCamera = float4(1,1, colorLerp * 0, colorLerp);

	//(directionalLightContribution * lambert + ambientLightContribution)* colorTexture
	//Default output will add dot product of light direction	

	PixelOutput returnValue;
	returnValue.myColor = colorCamera;

	return returnValue;
}