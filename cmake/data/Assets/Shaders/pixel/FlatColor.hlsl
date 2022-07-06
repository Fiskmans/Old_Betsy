#include "../ShaderStructs.hlsli"


PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput returnValue;
	returnValue.myColor = float4(1,0.92,0.92,1);

	return returnValue;
}