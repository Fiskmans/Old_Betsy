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

	PixelOutput returnValue;

	float channelMaskRed;
	channelMaskRed = 0;

	float channelMaskGreen;
	channelMaskGreen = MaterialMap.Sample(MeshTextureSampler, input.myUV2).g;

	float normalMask = NormalMap.Sample(MeshTextureSampler, input.myUV2).g * objectLifeTime;

	float channelMaskBlue;
	channelMaskBlue = 0;


	float onInputTriggerTime = totalTime - objectLifeTime;
	float timer = objectLifeTime * 10;

	float glowAmount = 1;

	float firstTimerValue = 0.3;
	float secondTimerValue = 2;
	float thirdTimerValue = 5;

	float masterGlowAmount = 1;

	//input.myUV2 = lerp(input.myUV, input.myUV1, sin(objectLifeTime / objectExpectedLifeTime) * 3);

	channelMaskGreen = MaterialMap.Sample(MeshTextureSampler, input.myUV2).g;
	channelMaskBlue = MaterialMap.Sample(MeshTextureSampler, input.myUV2).b;
	normalMask += channelMaskGreen * glowAmount;
	normalMask += channelMaskBlue * glowAmount;


	if (timer > firstTimerValue && timer < secondTimerValue)
	{

		channelMaskRed = saturate(MaterialMap.Sample(MeshTextureSampler, input.myUV2).r * (timer - firstTimerValue));
		normalMask += channelMaskRed * glowAmount;
	}

	else if (timer > secondTimerValue && timer < firstTimerValue)
	{
		channelMaskGreen -= (timer - secondTimerValue);
		channelMaskBlue -= (timer - secondTimerValue);
	}

	else if (timer > thirdTimerValue)
	{

		//channelMaskBlue -= timer - 10;
		channelMaskRed -= timer - thirdTimerValue;
	}


	float combined = channelMaskBlue + channelMaskRed;


	returnValue.myColor = AlbedoMap.Sample(MeshTextureSampler, input.myUV2) * masterGlowAmount;

	//float tintedChannelMaskRed = returnValue.myColor.r + (1 - returnValue.myColor.r) * 0.4f;
	//returnValue.myColor.r = tintedChannelMaskRed;

	returnValue.myColor.a = channelMaskGreen + (combined);


	return returnValue;
}