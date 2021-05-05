#include "Data/Shaders/ShaderStructs.hlsli"

PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput returnValue;
#ifdef VERTEXCOLOR
	returnValue.myColor = input.myColor;
#else
	returnValue.myColor = float4(1, 1, 1, 1);
#endif
	return returnValue;
}