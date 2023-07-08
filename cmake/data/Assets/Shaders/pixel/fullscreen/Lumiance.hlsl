#include "ShaderStructs.hlsli"

PixelOutput pixelShader(FullscreenVertexToPixel input)
{
	PixelOutput outp;
	float3 resource = texture0.Sample(Sampler, input.myUV.xy).rgb;

	float luminance = dot(resource, float3(0.2126f, 0.7152f, 0.0722f));
	const float cutOff = 0.8f;
	outp.myColor.rgb = float3(0, 0, 0);

	outp.myColor.rgb += resource * (luminance >= cutOff);

	outp.myColor.rgb += resource * pow(luminance / cutOff, 5) * (luminance < cutOff && luminance >= cutOff * 0.5f);
	outp.myColor.a = 1;

	return outp;
}