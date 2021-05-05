#include "SpriteShaderStructs.hlsli"

PixelOutput pixelShader(VertexToPixel input)
{
    PixelOutput returnValue;
    float4 textureColor = albedoTexture.Sample(defaultSampler, input.myUV.xy).rgba;
	//returnValue.myColor = color;
	returnValue.myColor = float4(textureColor.r * color.r, textureColor.g * color.g, textureColor.b * color.b, textureColor.a * color.a).rgba;
	returnValue.myColor = saturate(returnValue.myColor);

    return returnValue;
}