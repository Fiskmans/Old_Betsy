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
	input.myUV2 = lerp(input.myUV, input.myUV1, sin(objectLifeTime / objectExpectedLifeTime));

	if (onInputTriggerTime > 2)
	{
		onInputTriggerTime = 0;
	}


	float channelMaskR;
	channelMaskR = MaterialMap.Sample(MeshTextureSampler, input.myUV2).r;

	float channelMaskG;
	channelMaskG = MaterialMap.Sample(MeshTextureSampler, lerp(input.myUV, input.myUV1, sin(onInputTriggerTime /4) + 0.5)).g;

	float channelMaskB;
	channelMaskB = MaterialMap.Sample(MeshTextureSampler, lerp(input.myUV, input.myUV1, sin(onInputTriggerTime / 10) + 0.5)).b;
	


	float combined = channelMaskR - channelMaskG - channelMaskB;




	returnValue.myColor = AlbedoMap.Sample(MeshTextureSampler, input.myUV2);
	returnValue.myColor.a = combined - channelMaskBlue;

	//returnValue.myColor = combined * (1.0f-channelMaskBlue);
	//returnValue.myColor.a = combined * (1.0f - channelMaskBlue);
	//Blend highlighting the red channel for health visual que
	/*
	float val = abs(sin(totalTime));
	float4 v = float4(val, 0, 0, 1);
	returnValue.myColor = AlbedoMap.Sample(defaultSampler, input.myUV) + v;
	*/

	return returnValue;
}