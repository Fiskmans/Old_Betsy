#include "ShaderStructs.hlsli"

PixelOutput pixelShader(FullscreenVertexToPixel input)
{
	PixelOutput outp;
	outp.myColor.rgba = texture0.Sample(Sampler, input.myUV).rgba;
	if (outp.myColor.r >= 0.97f)
	{
		discard;
	}
	outp.myColor.a = outp.myColor.r;
	return outp;
}