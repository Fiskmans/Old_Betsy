#include "Data/Shaders/ShaderStructs.hlsli"
SamplerState defaultSampler : register(s0);

#ifndef MULTIPLE_UV
#error "model missing data"
#endif



PixelOutput pixelShader(VertexToPixel input)
{
	
	PixelOutput returnValue;

	returnValue.myColor = lerp(AlbedoMap.Sample(defaultSampler, input.myUV), AlbedoMap.Sample(defaultSampler, input.myUV1), cos(totalTime));

	return returnValue;
}