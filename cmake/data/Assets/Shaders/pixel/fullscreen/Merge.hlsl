#include "ShaderStructs.hlsli"

PixelOutput pixelShader(FullscreenVertexToPixel input)
{
	PixelOutput outp;

	float3 res = texture0.Sample(Sampler, input.myUV.xy).rgb;
	float4 res2 = texture1.Sample(Sampler, input.myUV.xy).rgba;

	outp.myColor.rgb = res2.rgb * res2.a + (res.rgb * (1.f - res2.a));
	outp.myColor.a = 1.f;
	return outp;
}