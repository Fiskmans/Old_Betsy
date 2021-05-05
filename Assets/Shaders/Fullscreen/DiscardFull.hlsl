#include "Data/Shaders/Fullscreen/FullscreenShaderStructs.hlsli"

PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput outp;
	outp.myColor.rgba = resource1.Sample(defaultSampler, input.myUV).rgba;
	if (outp.myColor.r >= 0.97f)
	{
		discard;
	}
	outp.myColor.a = outp.myColor.r;
	return outp;
}