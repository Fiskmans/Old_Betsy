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
	input.myUV2 = lerp(input.myUV, input.myUV1, sin(objectLifeTime));

	if (onInputTriggerTime > 2)
	{
		onInputTriggerTime = 0;
	}


	float channelMaskR;
	channelMaskR = MaterialMap.Sample(MeshTextureSampler, input.myUV2).r;

	float channelMaskG;
	channelMaskG = MaterialMap.Sample(MeshTextureSampler, input.myUV2).g;

	float channelMaskB;
	channelMaskB = MaterialMap.Sample(MeshTextureSampler, input.myUV2).b;
	


	float edgeTip = channelMaskR - channelMaskG;
	float combined = channelMaskR + (edgeTip);


	returnValue.myColor = AlbedoMap.Sample(MeshTextureSampler, input.myUV2);

	if (objectLifeTime < 0.18)
	{
		returnValue.myColor.a = channelMaskR + (edgeTip * 4);
	}

	else
	{
		returnValue.myColor.a = combined - (objectLifeTime * 2);
	}




	return returnValue;
}