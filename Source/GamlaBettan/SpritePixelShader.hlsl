#include "SpriteShaderStructs.hlsli"

PixelOutput main(VertexToPixel input)
{
    PixelOutput returnValue;
    float4 textureColor = albedoTexture.Sample(defaultSampler, input.myUV.xy).rgba;
    returnValue.myColor = textureColor;
    return returnValue;
}