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


	//returnValue.myColor = lerp(AlbedoMap.Sample(defaultSampler, input.myUV), AlbedoMap.Sample(defaultSampler, input.myUV1), cos(totalTime));

	//Blue mask for the static start of uvset2
	float channelMaskBlue;
	channelMaskBlue = MaterialMap.Sample(MeshTextureSampler, input.myUV2).b;

	float onInputTriggerTime = totalTime - objectLifeTime;
	input.myUV2 = lerp(input.myUV, input.myUV1, sin(objectLifeTime));

	if (onInputTriggerTime > 2)
	{
		onInputTriggerTime = 0;
	}




	//	float4 channelMask;
	//	channelMask = MaterialMap.Sample(defaultSampler, input.myUV2);

		//Divides the material channel into 3 channels for controlling the masks in different speeds
	float channelMaskR;
	channelMaskR = MaterialMap.Sample(MeshTextureSampler, lerp(input.myUV, input.myUV1, sin(objectLifeTime) * 1.1)).r;

	float channelMaskG;
	channelMaskG = MaterialMap.Sample(MeshTextureSampler, lerp(input.myUV, input.myUV1, sin(objectLifeTime) * 1.5)).g;

	float channelMaskB;
	channelMaskB = MaterialMap.Sample(MeshTextureSampler, lerp(input.myUV, input.myUV1, sin(objectLifeTime) * 2)).b;



	float combined = (channelMaskR + channelMaskG + channelMaskB) * 0.7;



	returnValue.myColor = AlbedoMap.Sample(MeshTextureSampler, input.myUV);

	if (objectLifeTime < 0.1)
	{
		returnValue.myColor.a = combined * 10;
	}

	else
	{
		returnValue.myColor.a = combined - (objectLifeTime * 2);
	}


	return returnValue;
}