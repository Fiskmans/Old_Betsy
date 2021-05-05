#include "Data/Shaders/ShaderStructs.hlsli"
SamplerState defaultSampler : register(s0);

#ifndef MULTIPLE_UV
#error "model missing data"
#endif



PixelOutput pixelShader(VertexToPixel input)
{
	//Panner for weapon slashes
	PixelOutput returnValue;


	//Divides the material channel into 3 channels for controlling the masks in different speeds
	float channelMaskR;
	channelMaskR = MaterialMap.Sample(defaultSampler, ((input.myUV * 1) + float2 ((0), (objectLifeTime * -1.5)))).r;

	float channelMaskG;
	channelMaskG = MaterialMap.Sample(defaultSampler, ((input.myUV * 1) + float2 ((0), (objectLifeTime * -1.5)))).g;

	float channelMaskB;
	channelMaskB = MaterialMap.Sample(defaultSampler, (input.myUV2)).b;

	float combinedAlphaOverTime = 0;

	if (objectLifeTime < 0.5)
	{
		combinedAlphaOverTime = channelMaskB;
	}

	else
	{
		combinedAlphaOverTime = channelMaskB - ((objectLifeTime - 0.5) * 0.07);
	}

	//channelMaskR = channelMaskR - (objectLifeTime*0.7);
	//combined = ((channelMaskG* 2) - channelMaskB);


	returnValue.myColor = (channelMaskR + channelMaskG) * float4(80, 0, 4, 0);
	returnValue.myColor.a = combinedAlphaOverTime * 40;



	return returnValue;
}