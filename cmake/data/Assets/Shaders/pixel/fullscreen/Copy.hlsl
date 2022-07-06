#include "../../FullscreenShaderStructs.hlsli"

PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput outp;
	outp.myColor.rgb = resource1.Sample(defaultSampler, input.myUV).rgb;
	outp.myColor.a = 1;
	return outp;
}