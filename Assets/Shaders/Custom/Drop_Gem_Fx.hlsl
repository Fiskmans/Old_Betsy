#include "Data/Shaders/ShaderStructs.hlsli"
SamplerState defaultSampler : register(s0);

#ifndef MULTIPLE_UV
#error "model missing data"
#endif



/*
PixelOutput pixelShader(VertexToPixel input)
{
	//Panner for weapon slashes
	PixelOutput returnValue;
	
	
	//Blue mask for the static start of uvset2
	float4 lerpValue;
	lerpValue = AlbedoMap.Sample(MeshTextureSampler, lerp(input.myUV + 0.5, input.myUV1 - 0.5, sin(totalTime * 8)));



	returnValue.myColor = lerpValue * 3;
	
	
	returnValue.myColor.a = 1;


	return returnValue;
}
*/

PixelOutput pixelShader(VertexToPixel input)
{
	//Panner for weapon slashes
	PixelOutput returnValue;


	//Blue mask for the static start of uvset2
	float4 color = AlbedoMap.Sample(defaultSampler, input.myUV2);
	float4 colorLerp = AlbedoMap.Sample(defaultSampler, lerp(input.myUV + 0.5, input.myUV1 - 0.5, sin(totalTime * 1.4)));

	float alphaLerp = MaterialMap.Sample(defaultSampler, lerp(input.myUV + 0.5, input.myUV1 - 0.5, sin(totalTime * 8))).r;

	returnValue.myColor = color + colorLerp * 1.5;
	returnValue.myColor.a = 3;


	return returnValue;
}