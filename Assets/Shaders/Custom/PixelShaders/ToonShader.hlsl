#include "Data/Shaders/ShaderStructs.hlsli"



PixelOutput pixelShader(VertexToPixel input)
{
#include "Data/Shaders/Sampling.hlsli"



	//Calculate the angle value for every pixel
	float intensity = dot(normalize(environmentLightDirection.xyz), normalize(input.myNormal.xyz));
	if (intensity < 0)
		intensity = 0;

	float4 color = albedoData * float4(environmentLightColor, 1) * enviromentLightIntensity;


	if (intensity > 0)
		color = float4(1.0 * color.r, 1.0 * color.g, 1.0 * color.b, color.a);

	else
		color = float4(0.9 * color.r, 0.9 * color.g, 0.9 * color.b, color.a);

	//(directionalLightContribution * lambert + ambientLightContribution)* colorTexture
	//Default output will add dot product of light direction	

	PixelOutput returnValue;
    returnValue.myColor = Shade(color, input);

	return returnValue;
}