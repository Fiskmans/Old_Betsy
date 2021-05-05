#include "ShaderStructs.hlsli"


PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput returnValue;

	returnValue.myColor.rgb = 0.3f;
	returnValue.myColor.a = AlbedoMap.Sample(defaultSampler, input.myUV).a;

	return returnValue;
}