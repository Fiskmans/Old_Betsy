#include "Data/Shaders/ShaderStructs.hlsli"
SamplerState defaultSampler : register(s0);

#ifndef MULTIPLE_UV
#error "model missing data"
#endif



SamplerState MeshTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

PixelOutput pixelShader(VertexToPixel input)
{
	//Panner for weapon slashes
	PixelOutput returnValue;
	
	float channelMaskBlue;
	channelMaskBlue = MaterialMap.Sample(MeshTextureSampler, input.myUV2).b;

	float onInputTriggerTime = totalTime - objectLifeTime;
	input.myUV2 = lerp(input.myUV, input.myUV1, sin(objectLifeTime / objectExpectedLifeTime));

	if (onInputTriggerTime > 2)
	{
		onInputTriggerTime = 0;
	}


	float channelMaskR;
	channelMaskR = MaterialMap.Sample(defaultSampler, ((input.myUV * 1) + float2 ((0), (objectLifeTime * -2)))).r;

	float channelMaskG;
	channelMaskG = MaterialMap.Sample(defaultSampler, ((input.myUV * 1) + float2 ((objectLifeTime * -1), (objectLifeTime * -2.7)))).g;

	float channelMaskB;
	channelMaskB = MaterialMap.Sample(defaultSampler, ((input.myUV * 1) + float2 ((objectLifeTime * 1), (objectLifeTime * -1.7)))).b;
	


	float combined = (channelMaskR * 3) + (channelMaskG * 1.2) + channelMaskB;



	returnValue.myColor = AlbedoMap.Sample(MeshTextureSampler, input.myUV2);
	returnValue.myColor.a = (combined) - channelMaskBlue;

	return returnValue;
}