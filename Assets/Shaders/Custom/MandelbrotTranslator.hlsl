#include "Data/Shaders/ShaderStructs.hlsli"

float3 HUEtoRGB(in float H)
{
	float R = abs(H * 6 - 3) - 1;
	float G = 2 - abs(H * 6 - 2);
	float B = 2 - abs(H * 6 - 4);
	return saturate(float3(R, G, B));
}

PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput returnValue;

	float val = MaterialMap.Sample(defaultSampler, input.myUV).r;
	if (val < 0.0)
	{
		returnValue.myColor = float4(0,0,0,1);
	}
	else
	{
		float fr = frac(val / 15.0);
		float ler = abs((fr-0.5) * 2);
		returnValue.myColor = float4(lerp(float3(1,0,1),float3(1,1,0),ler), 1);
	}

	return returnValue;
}