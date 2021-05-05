#include "Data/Shaders/ShaderStructs.hlsli"


PixelOutput pixelShader(VertexToPixel input)
{
#include "Data/Shaders/Sampling.hlsli"

	//Calculate the angle value for every pixel
	float intensity = dot(normalize(environmentLightDirection.xyz), normalize(input.myNormal.xyz));
	if (intensity < 0)
		intensity = 0;

	float panSpeed = 0.1;
	
	float mask = MaterialMap.Sample(defaultSampler, float2(input.myUV.x, input.myUV.y)).r;
	
	float maskTwo = MaterialMap.Sample(defaultSampler, float2(input.myUV.x  - (totalTime * panSpeed), input.myUV.y)).g;
	
	float timeScale = totalTime * .3;
	float scale = 0.15;
	float scaleMin = 2.5;
	
	float shiftingA = (sin(totalTime) + scaleMin) * scale;
	float shiftingB = (cos(sin(timeScale)) + scaleMin) * scale;
	float shifting = ((shiftingB * shiftingA) * shiftingB) + (shiftingB * 0.3) * ((sin(totalTime * 1) + 2) * 0.5);
	
	float4 masking = float4(0.02,0.02,0.02,1) + float4((mask * shifting), ((mask * shifting) * 0.65),((mask * shifting) * 0.2),0) * float4(maskTwo,maskTwo,maskTwo,1);
	float4 color = masking * float4(environmentLightColor, 1) * enviromentLightIntensity;
	
	

	if (intensity > 0)
		color = float4(1.0, 1.0, 1.0, color.a) * color;

	else
		color = float4(0.9, 0.9, 0.9, color.a) * color;

	//(directionalLightContribution * lambert + ambientLightContribution)* colorTexture
	//Default output will add dot product of light direction	
		
	PixelOutput returnValue;
	returnValue.myColor = color;

	return returnValue;
}