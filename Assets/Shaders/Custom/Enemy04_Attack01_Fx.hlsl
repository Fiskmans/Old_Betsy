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


	//+0.5 till sinus kurvan för att gå ifrån -0.5 till 1.5 i 0 - 1 uv ytan, jämnt emellan start och slut 
	/*
	if (input.myUV2.x < 0 || input.myUV2.x > 1)
	{
		discard;
	}
	*/


//	float4 channelMask;
//	channelMask = MaterialMap.Sample(defaultSampler, input.myUV2);

	//Divides the material channel into 3 channels for controlling the masks in different speeds
	float channelMaskR;
	channelMaskR = MaterialMap.Sample(MeshTextureSampler, input.myUV2).r;

	float channelMaskG;
	channelMaskG = MaterialMap.Sample(MeshTextureSampler, lerp(input.myUV, input.myUV1, sin(onInputTriggerTime /4) + 0.5)).g;

	float channelMaskB;
	channelMaskB = MaterialMap.Sample(MeshTextureSampler, lerp(input.myUV, input.myUV1, sin(onInputTriggerTime / 10) + 0.5)).b;
	


	float combined = channelMaskR - channelMaskG - channelMaskB;

	//float redMaskChannel = channelMask.x;
	//float greenMaskChannel = channelMask.y;
	//float blueMaskChannel = channelMask.z;


	//float redMaskChannelPanner = channelMask.x + (totalTime * 10);


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