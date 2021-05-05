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

	float mask_matR = MaterialMap.Sample(defaultSampler, ((input.myUV * 1) + float2 ((objectLifeTime * .05), (objectLifeTime * .0)))).r;
	float mask_matG = MaterialMap.Sample(defaultSampler, ((input.myUV * 3) + float2((objectLifeTime * .02), (objectLifeTime * .075)))).g; //perlinNoise
	float mask_matB = MaterialMap.Sample(defaultSampler, ((input.myUV * 5) + float2((objectLifeTime * -.07), (objectLifeTime * 0.2)))).b;
	float mask_matA = MaterialMap.Sample(defaultSampler, ((input.myUV * 1) + (0))).a;
	float4 colorTint = float4 (.14, 0.65, .2, 1);

	//returnValue.myColor = 0;
	//returnValue.myColor.a = (.3 + mask_matB / mask_matR) * mask_matA;
	returnValue.myColor = AlbedoMap.Sample(defaultSampler, input.myUV) * colorTint;
	returnValue.myColor.a = (0.2 + (mask_matB) * ( 0.5 + mask_matG * 1)) * (0.2 + mask_matR) * mask_matA;
	//returnValue.myColor.a = 1 * mask_matA;

	return returnValue;
}