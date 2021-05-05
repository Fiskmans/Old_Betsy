#include "Data/Shaders/ShaderStructs.hlsli"


SamplerState defaultSampler : register(s0);
PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput returnValue;
	int channel = (input.myUV.x * 3.0 + totalTime / 20.0) % 3.0;
	float val = PerlinNoise.Sample(defaultSampler, input.myUV)[channel];

	switch (channel)
	{
	case 0:
		returnValue.myColor = float4(val, val*0.5, val*0.5, 1);
		break;
	case 1:
		returnValue.myColor = float4(val*0.5, val * 0.5, val, 1);
		break;
	case 2:
		returnValue.myColor = float4(val*0.5, val, val * 0.5, 1);
		break;
	default:
		returnValue.myColor = float4(1, 1, 1, 1);
		break;
	}
	return returnValue;
}