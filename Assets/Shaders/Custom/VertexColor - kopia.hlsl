#include "Data/Shaders/ShaderStructs.hlsli"

PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput returnValue;
	float val = abs(sin(totalTime));
	returnValue.myColor = float4(val, val, val, 1);
	return returnValue;
}