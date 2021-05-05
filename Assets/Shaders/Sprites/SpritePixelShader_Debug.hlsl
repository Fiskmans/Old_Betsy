#include "SpriteShaderStructs.hlsli"

PixelOutput pixelShader(VertexToPixel input)
{
    PixelOutput returnValue;
	returnValue.myColor = float4(1,1,1,1) * color;
    return returnValue;
}