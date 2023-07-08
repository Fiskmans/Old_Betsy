#include "ShaderStructs.hlsli"

PixelOutput pixelShader(FullscreenVertexToPixel input)
{
	PixelOutput outp;
	outp.myColor.rgb = texture0.Sample(Sampler, input.myUV).rgb;
	outp.myColor.a = 1;
	return outp;
}