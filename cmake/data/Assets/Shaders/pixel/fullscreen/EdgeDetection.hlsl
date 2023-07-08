#include "ShaderStructs.hlsli"


static const float HorizontalKernel[3][3] =
{
	{-1,0,1},
	{-2,0,2},
	{-1,0,1}
};

static const float VerticalKernel[3][3] =
{
	{-1,-2,-1},
	{0,0,0},
	{1,2,1}
};


float Flatten(float3 aValue)
{
	return aValue.x + aValue.y + aValue.z;
}

PixelOutput pixelShader(FullscreenVertexToPixel input)
{
	PixelOutput outp;

	float3 textureSize = GetTextureSize(texture0);
	float2 pixelSize = float2(1.0/float(textureSize.x), 1.0/float(textureSize.y));


	float2 Edgyness = float2(0, 0);


	[unroll] 
	for (int x = -1; x < 2; x++)
	{
		[unroll] 
		for (int y = -1; y < 2; y++)
		{
			float value = Flatten(texture0.Sample(Sampler, input.myUV.xy + float2(x* pixelSize.x,y* pixelSize.y)).rgb);

			Edgyness.x += HorizontalKernel[x + 1][y + 1] * value;
			Edgyness.y += VerticalKernel[x + 1][y + 1] * value;
		}
	}


	float edge = length(Edgyness);

	outp.myColor.rgb = float3(edge, edge, edge);
	outp.myColor.a = 1;

	return outp;
}