#include "Data/Shaders/ShaderStructs.hlsli"
SamplerState defaultSampler : register(s0);

#ifndef MULTIPLE_UV
#error "model missing data"
#endif


PixelOutput pixelShader(VertexToPixel input)
{
	//Panner for weapon slashes
	PixelOutput returnValue;

	//Partikel scroll effect
	float channelMaskRed = MaterialMap.Sample(defaultSampler, input.myUV2 + float2(objectLifeTime * 10, 2)).r;

	float channelMaskGreen = MaterialMap.Sample(defaultSampler, input.myUV2 + float2(objectLifeTime * -10, -2)).g;

	float channelMaskBlue = MaterialMap.Sample(defaultSampler, input.myUV2 + float2(objectLifeTime * 3, 0)).b;

	float channelMaskAlpha = MaterialMap.Sample(defaultSampler, input.myUV2 + float2(objectLifeTime * 10, 0)).a;

	//emissive over time
	float emissive = 0;
	float combinedAlphaOverTime = (channelMaskRed + channelMaskGreen + channelMaskBlue) * channelMaskAlpha;





	returnValue.myColor = AlbedoMap.Sample(defaultSampler, input.myUV2);
	returnValue.myColor.a = combinedAlphaOverTime;


	return returnValue;
}