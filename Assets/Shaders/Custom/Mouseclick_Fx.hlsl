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
	

	input.myUV2 = lerp(input.myUV, input.myUV1, sin(objectLifeTime*1.7));



	//Divides the material channel into 3 channels for controlling the masks in different speeds
	float channelMaskR;
	channelMaskR = MaterialMap.Sample(defaultSampler, input.myUV2).r;

	float channelMaskG;
	channelMaskG = MaterialMap.Sample(defaultSampler, input.myUV2).g;

	float channelMaskB;
	channelMaskB = MaterialMap.Sample(defaultSampler, input.myUV2).b;
	


	float combined = channelMaskR * channelMaskG * channelMaskB;




	returnValue.myColor = AlbedoMap.Sample(defaultSampler, input.myUV2);
	returnValue.myColor.a = combined * (objectLifeTime * 20);



	return returnValue;
}