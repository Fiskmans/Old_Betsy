#include "ShaderStructs.hlsli"
PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput returnValue;
	returnValue.myColor.rgba = float4(input.myPosition.zzz,0);
	return returnValue;
}