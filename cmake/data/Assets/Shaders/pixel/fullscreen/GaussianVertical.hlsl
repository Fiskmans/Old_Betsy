#include "../../FullscreenShaderStructs.hlsli"

PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput outp;

	float texelSize = 1.f / (float)(GetTextureSize(resource1).y);
	float3 blurColor = float3(0.f, 0.f, 0.f);

	float start = (((float)(kernelSize)-1.f) * 0.5f) * -1.f;

	float2 uv;
	float3 resource;

	for (unsigned int i = 0; i < kernelSize; i++)
	{
		uv = input.myUV.xy + float2(0.f, texelSize * (start + (float)(i)));
		resource = resource1.Sample(defaultSampler, uv).rgb;
		blurColor += resource * GaussianKernel[i];
	}

	outp.myColor.rgb = blurColor;
	outp.myColor.a = 1.f;

	return outp;
}