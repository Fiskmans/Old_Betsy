#include "../../FullscreenShaderStructs.hlsli"

PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput outp;

	float3 res = resource1.Sample(defaultSampler, input.myUV.xy).rgb;
	float4 res2 = resource2.Sample(defaultSampler, input.myUV.xy).rgba;

	outp.myColor.rgb = res2.rgb * res2.a + (res.rgb * (1.f - res2.a));
	outp.myColor.a = 1.f;
	return outp;
}