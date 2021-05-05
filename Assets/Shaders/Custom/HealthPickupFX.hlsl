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
	
	//Version 1
	/*
	//	float4 channelMask;
	//	channelMask = MaterialMap.Sample(defaultSampler, input.myUV2);

	//Divides the material channel into 3 channels for controlling the masks in different speeds
	float channelMaskR;
	channelMaskR = MaterialMap.Sample(MeshTextureSampler, input.myUV2).r;

	float channelMaskG;
	channelMaskG = MaterialMap.Sample(MeshTextureSampler, input.myUV2).g;

	float channelMaskB;
	channelMaskB = MaterialMap.Sample(MeshTextureSampler, input.myUV2).b;
	
	channelMaskR = abs(channelMaskR - 1);

	//float combined = channelMaskR - channelMaskG - channelMaskB;


	//returnValue.myColor = AlbedoMap.Sample(MeshTextureSampler, input.myUV2);
	//returnValue.myColor.a = channelMaskR;
	//Blend highlighting the red channel for health visual que
	
	float val = abs(sin(totalTime));
	float4 v = float4(val, 0, 0, 1);
	returnValue.myColor.r = AlbedoMap.Sample(MeshTextureSampler, input.myUV2).r + val;
	returnValue.myColor.a = channelMaskR;

	returnValue.myColor.g = AlbedoMap.Sample(MeshTextureSampler, input.myUV2).g;
	returnValue.myColor.a = channelMaskR + channelMaskG;

	returnValue.myColor.b = AlbedoMap.Sample(MeshTextureSampler, input.myUV2).b;
	returnValue.myColor.a = channelMaskR + channelMaskG + channelMaskB;
	//returnValue.myColor.a = channelMaskG + channelMaskB;
	*/





	//Version 2
	/*
	float4 material = MaterialMap.Sample(MeshTextureSampler, input.myUV2);
	float negatedRedMaterial = 1.0f - material.r;
	float redStrength = 1.0f;

	float3 albedoRedStrength = pow(1.0f - AlbedoMap.Sample(MeshTextureSampler, input.myUV).rrr, 10) * 10000;
	float bumpiness = albedoRedStrength.r;

	returnValue.myColor.rgb = AlbedoMap.Sample(MeshTextureSampler, input.myUV).rgb;
	returnValue.myColor.r += abs(cos(totalTime)) * (negatedRedMaterial * redStrength) * bumpiness;
	


	returnValue.myColor.a = 1.0f;
	*/
	

	//Version 3
	float4 material = MaterialMap.Sample(MeshTextureSampler, input.myUV2);
	float negatedRedMaterial = 1.0f - material.r;
	float redStrength = 0.8f;

	float3 albedoRedStrength = (1.0f - AlbedoMap.Sample(MeshTextureSampler, input.myUV).rrr) * redStrength;
	float bumpiness = albedoRedStrength.r;

	returnValue.myColor.rgb = AlbedoMap.Sample(MeshTextureSampler, input.myUV).rgb;
	returnValue.myColor.r += abs(cos(totalTime/7)) * (negatedRedMaterial * redStrength) * bumpiness * 3;



	returnValue.myColor.a = 1.0f;


	return returnValue; 
}