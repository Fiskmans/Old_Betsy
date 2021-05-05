#include "Data/Shaders/Fullscreen/FullscreenShaderStructs.hlsli"

PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput outp;

	float texelSize = 1.f / (float)(GetTextureSize(resource1).y);
	float3 blurColor = float3(0.f, 0.f, 0.f);

	float start = (((float)(kernelSize2)-1.f) * 0.5f) * -1.f;

	float2 uv;
	float3 resource;

	for (unsigned int i = 0; i < kernelSize2; i++)
	{
		uv = input.myUV.xy + float2(0.f, texelSize * (start + (float)(i)));
		resource = resource1.Sample(defaultSampler, uv).rgb;
		blurColor += resource * GaussianKernel2[i];
	}

	float3 flatColor = resource1.Sample(defaultSampler, input.myUV.xy).rgb;
	float condition = resource2.Sample(defaultSampler, input.myUV.xy).r;

	outp.myColor.rgb = lerp(flatColor, blurColor, condition);
	outp.myColor.a = 1.f;

	return outp;
}