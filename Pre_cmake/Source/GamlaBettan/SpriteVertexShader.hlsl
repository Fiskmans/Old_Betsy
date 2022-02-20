#include "SpriteShaderStructs.hlsli"

VertexToPixel main(VertexInput input)
{
    VertexToPixel returnValue;
    returnValue.myPosition = mul(toWorld, input.myPosition);
    returnValue.myUV = input.myUV;
    return returnValue;
}