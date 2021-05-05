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

	float colorR = 0;
	float colorG = 0;
	float colorB = 0;

	float channelMaskRed = 0;
	float channelMaskBlue = 0;
	float channelMaskGreen = 0;

	float channelMaskRed1 = 0;
	float channelMaskBlue1 = 0;
	float channelMaskGreen1 = 0;


	float normalMask = NormalMap.Sample(MeshTextureSampler, input.myUV2).g * objectLifeTime;



	float onInputTriggerTime = totalTime - objectLifeTime;

	float timer = objectLifeTime * 10;

	float glowAmount = 1;

	float firstTimerValue = 0;
	float secondTimerValue = 0.7;
	float thirdTimerValue = 1.4;
	float fourthTimerValue = 5;

	float masterGlowAmount = 1;

	//Take away the lower part when implemeting
	colorR = AlbedoMap.Sample(MeshTextureSampler, input.myUV2).r;
	colorG = AlbedoMap.Sample(MeshTextureSampler, input.myUV2).g;
	colorB = AlbedoMap.Sample(MeshTextureSampler, input.myUV2).b;

	channelMaskRed = AlbedoMap.Sample(MeshTextureSampler, input.myUV2).r;
	channelMaskGreen = AlbedoMap.Sample(MeshTextureSampler, input.myUV2).g;
	channelMaskBlue = AlbedoMap.Sample(MeshTextureSampler, input.myUV2).b *0.4;
	normalMask += channelMaskGreen * glowAmount;
	normalMask += channelMaskBlue * glowAmount;


	if (timer > firstTimerValue && timer < secondTimerValue)
	{
		colorR = saturate(AlbedoMap.Sample(MeshTextureSampler, input.myUV2).r * (timer - firstTimerValue)) * 5;
		colorG = saturate(AlbedoMap.Sample(MeshTextureSampler, input.myUV2).g * (timer - firstTimerValue)) * 2;
		colorB = saturate(AlbedoMap.Sample(MeshTextureSampler, input.myUV2).b * (timer - firstTimerValue)) * 1.7;

		channelMaskRed = saturate(AlbedoMap.Sample(MeshTextureSampler, input.myUV2).r * (timer - firstTimerValue)) * 2;
		channelMaskGreen = saturate(AlbedoMap.Sample(MeshTextureSampler, input.myUV2).g * (timer - firstTimerValue)) * 1;
		channelMaskBlue = saturate(AlbedoMap.Sample(MeshTextureSampler, input.myUV2).b * (timer - firstTimerValue)) * 1;

		
		channelMaskRed1 = saturate(MaterialMap.Sample(MeshTextureSampler, input.myUV2).r * (timer - firstTimerValue) * 1.3);
		channelMaskGreen1 = saturate(MaterialMap.Sample(MeshTextureSampler, input.myUV2).g * (timer - firstTimerValue) * 1.3);
		channelMaskBlue1 = saturate(MaterialMap.Sample(MeshTextureSampler, input.myUV2).b * (timer - firstTimerValue) * 1.3);
		
		normalMask += channelMaskRed * glowAmount;
	}



	else if (timer >= secondTimerValue && timer < thirdTimerValue)
	{

		channelMaskBlue -= (timer  - secondTimerValue) * 3;
		channelMaskRed -= (timer - secondTimerValue) * 3;
	}



	float combinedAlpha = channelMaskBlue + channelMaskGreen + channelMaskRed;


	returnValue.myColor.r = (colorR * masterGlowAmount) + channelMaskRed1;
	returnValue.myColor.g = (colorG * masterGlowAmount);
	returnValue.myColor.b = (colorB * masterGlowAmount) + channelMaskBlue1;

	//returnValue.myColor = AlbedoMap.Sample(MeshTextureSampler, input.myUV2) * masterGlowAmount;
	

	//float tintedChannelMaskRed = returnValue.myColor.r + (1 - returnValue.myColor.r) * 0.4f;
	//returnValue.myColor.r = tintedChannelMaskRed;

	returnValue.myColor.a = combinedAlpha;


	return returnValue;
}