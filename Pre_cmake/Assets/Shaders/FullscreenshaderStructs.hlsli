

struct VertexInput
{
	unsigned int myIndex : SV_VertexID;
};

struct VertexToPixel
{
	float4 myPosition	:	SV_POSITION;
	float2 myUV			:	UV;
};

struct PixelOutput
{
	float4 myColor		:	SV_TARGET;
};



SamplerState defaultSampler : register(s0);

Texture2D resource_temp[16] : register(t0);
Texture2D resource1 : register(t0);
Texture2D resource2 : register(t1);
Texture2D resource3 : register(t2);
Texture2D resource4 : register(t3);
Texture2D resource5 : register(t4);
Texture2D resource6 : register(t5);
Texture2D resource7 : register(t6);
Texture2D resource8 : register(t7);
Texture2D resource9 : register(t8);
Texture2D resource10 : register(t9);
Texture2D resource11 : register(t10);
Texture2D resource12 : register(t11);


Texture2D LUTAutum : register(t1);
Texture2D LUTWinter : register(t2);

static const unsigned int kernelSize = 5;
static const float GaussianKernel[kernelSize] = {0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136f };

static const unsigned int kernelSize2 = 3;
static const float GaussianKernel2[kernelSize2] = { 1.0 / 4.0, 1.0 / 2.0, 1.0 / 4.0 };

int3 GetTextureSize(Texture2D tex)
{
	int width = 0;
	int height = 0;
	int numMips = 0;

	tex.GetDimensions(0, width, height, numMips);
	return int3(width, height, numMips);
}