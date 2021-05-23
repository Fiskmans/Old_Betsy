#include "../../FullscreenShaderStructs.hlsli"

PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput outp;

	float3 res = resource1.Sample(defaultSampler, input.myUV.xy).rgb;
	float3 res2 = resource2.Sample(defaultSampler, input.myUV.xy).rgb;


	float luminance = dot(res, float3(0.2126f, 0.7152f, 0.0722f));
	res2 *= (1.0f - luminance);
	outp.myColor.rgb = res + res2;
	outp.myColor.a = 1;

	return outp;
}