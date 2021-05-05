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
	channelMaskR = MaterialMap.Sample(defaultSampler, ((input.myUV * 1) + float2 ((0), (objectLifeTime * 1.8)))).r;

	float channelMaskG;
	channelMaskG = MaterialMap.Sample(defaultSampler, ((input.myUV * 1) + float2 ((0), (objectLifeTime * 1.8)))).g;

	float channelMaskB;
	channelMaskB = MaterialMap.Sample(defaultSampler, (input.myUV2)).b;






	//channelMaskR = channelMaskR - (objectLifeTime*0.7);
	//combined = ((channelMaskG* 2) - channelMaskB);


	returnValue.myColor = AlbedoMap.Sample(defaultSampler, input.myUV2);
	returnValue.myColor.a = (channelMaskG + channelMaskR * 30) * (channelMaskB * 0.7);



	return returnValue;
}