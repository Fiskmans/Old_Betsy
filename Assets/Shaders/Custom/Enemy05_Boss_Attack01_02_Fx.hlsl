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
	float channelMaskRed;
	channelMaskRed = MaterialMap.Sample(defaultSampler, input.myUV2 + float2(0, objectLifeTime * 2)).r;
	float channelMaskGreen = MaterialMap.Sample(defaultSampler, input.myUV2).g;
	float partikelBlend = channelMaskRed * channelMaskGreen;

	//scroll the blue layer horizontally
	float verticalBlueChannelMovement;
	verticalBlueChannelMovement = objectLifeTime * 1.7;
	float channelMaskBlue = MaterialMap.Sample(defaultSampler, input.myUV2 + float2(verticalBlueChannelMovement, objectLifeTime * -0.2)).b;
	float horizontalNoiseMask = channelMaskGreen * channelMaskBlue;

	float channelMaskAlpha = MaterialMap.Sample(defaultSampler, input.myUV2).a;

	//emissive over time
	float emissive = 0;
	float combinedAlphaOverTime = 0;

	if (objectLifeTime < 1.2)
	{
		emissive = objectLifeTime * (channelMaskAlpha*3);
		combinedAlphaOverTime = (partikelBlend * emissive) + (horizontalNoiseMask * (emissive * 0.1));
	}
	
	else if (objectLifeTime < 1.6)
	{
		combinedAlphaOverTime = (partikelBlend * (channelMaskAlpha * 30)) + (horizontalNoiseMask * (channelMaskAlpha  * 25));
	}

	else
	{
		combinedAlphaOverTime = (partikelBlend - ((objectLifeTime - 1.6) *0.3 )) + (horizontalNoiseMask - ((objectLifeTime - 1.6) * 0.3));
	}


	returnValue.myColor = AlbedoMap.Sample(defaultSampler, input.myUV2);
	returnValue.myColor.a = combinedAlphaOverTime;


	return returnValue;
}