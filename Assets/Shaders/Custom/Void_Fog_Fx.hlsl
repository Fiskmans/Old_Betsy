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
	
	//input.myUV[0] += (objectLifeTime*0.05);
	//input.myUV2[0] += (frac(objectLifeTime));

	float mask_matR = MaterialMap.Sample(defaultSampler, ((input.myUV * 4) + float2 ((objectLifeTime * -.002),(objectLifeTime * .02)))).r;
	float mask_matG = MaterialMap.Sample(defaultSampler, ((input.myUV * 4) + float2((objectLifeTime * .021),(objectLifeTime * .001)))).g; //perlinNoise
	float mask_matB = MaterialMap.Sample(defaultSampler, ((input.myUV * 4) + float2((objectLifeTime * .01),(objectLifeTime * .01)))).b;
	float mask_matA = MaterialMap.Sample(defaultSampler, ((input.myUV * 1) + (0))).a;
	float4 colorTint = float4 (0.6, .5, 0.9, 1);

	//returnValue.myColor = 0;
	//returnValue.myColor.a = (.3 + mask_matB / mask_matR) * mask_matA;
	returnValue.myColor = AlbedoMap.Sample(defaultSampler, input.myUV)* colorTint;
	returnValue.myColor.a = ((mask_matB + (mask_matR * 1)) / 2)* ((mask_matG + 1) / 1.5)* mask_matA;
	//returnValue.myColor.a = 1 * mask_matA;

	return returnValue;

}