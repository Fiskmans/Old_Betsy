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

	float mask_matR = MaterialMap.Sample(defaultSampler, ((input.myUV * 10) + (objectLifeTime * .02))).r;
	float mask_matG = MaterialMap.Sample(defaultSampler, ((input.myUV * 10) + (objectLifeTime * .1))).g; //perlinNoise
	float mask_matB = MaterialMap.Sample(defaultSampler, ((input.myUV * 10) + (objectLifeTime * .01))).b;
	float mask_matA = MaterialMap.Sample(defaultSampler, ((input.myUV * 1) + (0))).a;
	float4 colorTint = float4 (.5, 0.5, 1, 1);

	float hej = .2;


	returnValue.myColor = 0;
	returnValue.myColor.a = (mask_matR + 0.95) * mask_matA;

	return returnValue;

}