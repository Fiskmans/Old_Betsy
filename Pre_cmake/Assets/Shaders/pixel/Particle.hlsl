#include "../ParticleShaderStructs.hlsli"

PixelOutput pixelShader(GeometryToPixel input)
{
	PixelOutput output;
    
	output.myColor.rgba = input.myColor.rgba * Texture.Sample(DefaultSampler, input.myUV.xy);
	
    //output.myColor.rg = input.myUV.xy;
    //output.myColor.ba = float2(0, 1);
    return output;
}