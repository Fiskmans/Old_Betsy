
#if HAS_BONES && (BONESPERVERTEX == 0)
#error "Invalid flags, has no bones"
#endif

#if HAS_UV_SETS && (UV_SETS_COUNT == 0)
#error "Invalid flags, has no uv sets"
#endif

struct VertexInput
{
	float4 myPosition					:	POSITION;
	float4 myNormal						:	NORMAL;
	float4 myTangent					:	TANGENT;
	float4 myBiTangent					:	BITANGENT;
#ifdef VERTEXCOLOR
	float4 myColor						:	COLOR;
#endif
#if HAS_UV_SETS
	float2 myUV[UV_SETS_COUNT]			:	UV;
#endif
#ifdef HAS_BONES
	uint myBones[BONESPERVERTEX]		:	BONES;
	float myBoneWeights[BONESPERVERTEX]	:	BONEWEIGHTS;
#endif
};

struct VertexToPixel
{
	float4 myPosition					:	SV_POSITION;
	float4 myWorldPos					:	POSITION;
	float4 myNormal						:	NORMAL;
	float4 myTangent					:	TANGENT;
	float4 myBiTangent					:	BITANGENT;
#if HAS_UV_SETS
	float2 myUV[UV_SETS_COUNT]			:	UV;
#endif
#ifdef VERTEXCOLOR
	float4 myColor						:	COLOR;
#endif
};

struct PixelOutput
{
	float4 myColor		:	SV_TARGET;
};


cbuffer frameData : register(b0)
{
	float4x4 worldToCameraMatrix;
	float4x4 cameraToProjectionMatrix;
	float3 environmentLightDirection;
	float shadowIntensity;
	float3 environmentLightColor;
	float enviromentLightIntensity;
	float3 cameraPosition;
	float totalTime;
	float4x4 worldToShadowCamera;
	float4x4 shadowCameratoShadowProjection;
	float3 cameraDirection;
	float trash_0;
};

cbuffer objectData : register(b1)
{
	struct PointLight
	{
		float3 position;
		float intensity;
		float3 color;
		float range;
	} pointLights[NUMBEROFPOINTLIGHTS];
	float4x4 modelToWorldMatrix;
	float4 tint;
	uint myBoneOffsetIndex;
	uint numOfUsedPointLights;
	float objectLifeTime;
	uint objectSeed;
    float3 diffuseColor;
	float trash;
};

cbuffer BoneBuffer : register(b2)
{
	float4x4 myBoneTransforms[NUMBEROFANIMATIONBONES];
}


Texture2D AlbedoMap						: register(t0);
Texture2D NormalMap						: register(t1);
Texture2D MaterialMap					: register(t2);
TextureCube SkyBox						: register(t3);
TextureCube EnvironmentLight			: register(t4);
Texture2D BoneTexture					: register(t5);

Texture2D PerlinNoise					: register(t7);

Texture2D WorldPosition					: register(t8);

Texture2D ShadowMapping					: register(t9);

SamplerState defaultSampler				: register(s0);

static const float2 PoissonSamples[64] =
{
			float2(-0.5119625f, -0.4827938f),
			float2(-0.2171264f, -0.4768726f),
			float2(-0.7552931f, -0.2426507f),
			float2(-0.7136765f, -0.4496614f),
			float2(-0.5938849f, -0.6895654f),
			float2(-0.3148003f, -0.7047654f),
			float2(-0.42215f, -0.2024607f),
			float2(-0.9466816f, -0.2014508f),
			float2(-0.8409063f, -0.03465778f),
			float2(-0.6517572f, -0.07476326f),
			float2(-0.1041822f, -0.02521214f),
			float2(-0.3042712f, -0.02195431f),
			float2(-0.5082307f, 0.1079806f),
			float2(-0.08429877f, -0.2316298f),
			float2(-0.9879128f, 0.1113683f),
			float2(-0.3859636f, 0.3363545f),
			float2(-0.1925334f, 0.1787288f),
			float2(0.003256182f, 0.138135f),
			float2(-0.8706837f, 0.3010679f),
			float2(-0.6982038f, 0.1904326f),
			float2(0.1975043f, 0.2221317f),
			float2(0.1507788f, 0.4204168f),
			float2(0.3514056f, 0.09865579f),
			float2(0.1558783f, -0.08460935f),
			float2(-0.0684978f, 0.4461993f),
			float2(0.3780522f, 0.3478679f),
			float2(0.3956799f, -0.1469177f),
			float2(0.5838975f, 0.1054943f),
			float2(0.6155105f, 0.3245716f),
			float2(0.3928624f, -0.4417621f),
			float2(0.1749884f, -0.4202175f),
			float2(0.6813727f, -0.2424808f),
			float2(-0.6707711f, 0.4912741f),
			float2(0.0005130528f, -0.8058334f),
			float2(0.02703013f, -0.6010728f),
			float2(-0.1658188f, -0.9695674f),
			float2(0.4060591f, -0.7100726f),
			float2(0.7713396f, -0.4713659f),
			float2(0.573212f, -0.51544f),
			float2(-0.3448896f, -0.9046497f),
			float2(0.1268544f, -0.9874692f),
			float2(0.7418533f, -0.6667366f),
			float2(0.3492522f, 0.5924662f),
			float2(0.5679897f, 0.5343465f),
			float2(0.5663417f, 0.7708698f),
			float2(0.7375497f, 0.6691415f),
			float2(0.2271994f, -0.6163502f),
			float2(0.2312844f, 0.8725659f),
			float2(0.4216993f, 0.9002838f),
			float2(0.4262091f, -0.9013284f),
			float2(0.2001408f, -0.808381f),
			float2(0.149394f, 0.6650763f),
			float2(-0.09640376f, 0.9843736f),
			float2(0.7682328f, -0.07273844f),
			float2(0.04146584f, 0.8313184f),
			float2(0.9705266f, -0.1143304f),
			float2(0.9670017f, 0.1293385f),
			float2(0.9015037f, -0.3306949f),
			float2(-0.5085648f, 0.7534177f),
			float2(0.9055501f, 0.3758393f),
			float2(0.7599946f, 0.1809109f),
			float2(-0.2483695f, 0.7942952f),
			float2(-0.4241052f, 0.5581087f),
			float2(-0.1020106f, 0.6724468f),
};

float CloudShadow(float3 aGroundPosition)
{
	float perlin1 = PerlinNoise.Sample(defaultSampler, float2(20.232, 15.25) + aGroundPosition.xz / 150000 + float2(totalTime / 2000, 0)).r;
	float perlin2 = PerlinNoise.Sample(defaultSampler, float2(20.232, 15.25) + aGroundPosition.xz / 150000 + float2(0, totalTime / 200000)).r;
	float perlin = (perlin1 + perlin2) / 2;
	if (perlin > shadowIntensity)
	{
		return 0;
	}
	return clamp(0, 0.35, (shadowIntensity - perlin) * 3.5 / shadowIntensity);
}

float GetOcclusion(VertexToPixel input)
{
	float4 camPosition = mul(worldToShadowCamera, input.myWorldPos);
	float4 camProjection = mul(shadowCameratoShadowProjection, camPosition);

	float occlusion = 0;
	if (camProjection.x < -1 || camProjection.x > 1 || camProjection.y < -1 || camProjection.y > 1 || camProjection.z < -1 || camProjection.z > 1)
	{
		occlusion = 1;
	}

	float2 camUV = camProjection.xy;
	camUV *= 0.5;
	camUV += float2(0.5, 0.5);
	camUV.y *= -1;
	camUV.y += 1;

	float sampleSize = 0.0004;
	float bias = 0.002;
	float fuzzyOcclusion = 0;

	float delta = 0.0;
	[unroll] for (int i = 0; i < 64; i++)
	{
		float localDepth = ShadowMapping.Sample(defaultSampler, camUV + PoissonSamples[i] * sampleSize).r;
		delta += localDepth - camProjection.z;
		float localDelta = abs(camProjection.z - localDepth);
		fuzzyOcclusion += localDelta > bias;
	}

	const float threshhold = 0.2;
	if (delta > threshhold)
	{
		delta -= threshhold;
		delta *= (1 - threshhold);
		//returnValue.myColor = float4(1, 0, 0, 1) * delta;
		//return returnValue;
		fuzzyOcclusion = lerp(fuzzyOcclusion, 0, delta);
	}
	fuzzyOcclusion /= 64;
	fuzzyOcclusion += occlusion;
	fuzzyOcclusion = saturate(fuzzyOcclusion);
	return fuzzyOcclusion;
}

float4 Shade(float4 color, VertexToPixel input)
{
	return lerp(color, color * float4(0.4, 0.4, 0.7, 1), GetOcclusion(input));
}