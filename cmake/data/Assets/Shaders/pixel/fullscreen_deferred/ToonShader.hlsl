#include "ShaderStructs.hlsli"

float CloudShadow(float3 aGroundPosition)
{
    float perlin1 = Perlin.Sample(Sampler,float2(20.232,15.25) + aGroundPosition.xz / 150000 + float2(DeferredFrameBuffer.time / 2000, 0)).r;
    float perlin2 = Perlin.Sample(Sampler, float2(20.232, 15.25) + aGroundPosition.xz / 150000 + float2(0, DeferredFrameBuffer.time / 200000)).r;
    float perlin = (perlin1 + perlin2) / 2;
    if (perlin > DeferredFrameBuffer.cloudIntensity)
    {
        return 0;
    }
    return clamp(0, 0.35, (DeferredFrameBuffer.cloudIntensity - perlin) * 3.5 / DeferredFrameBuffer.cloudIntensity);
}

PixelOutput pixelShader(FullscreenVertexToPixel input)
{
	PixelOutput returnValue;

	if (Depth.Sample(Sampler, input.myUV).r == 1)
		discard;

	float4 worldPosition = float4(Position.Sample(Sampler, input.myUV).rgb, 1);

	
	float4 camPosition = mul(DeferredFrameBuffer.myToCamera, worldPosition);
	float4 camProjection = mul(DeferredFrameBuffer.myCameraProjection, camPosition);

	returnValue.myColor = camPosition;
	
	/*
	float occlusion = 0;
	if (camProjection.x < -1 || camProjection.x > 1 || camProjection.y < -1 || camProjection.y > 1 || camProjection.z < -1 || camProjection.z > 1)
	{
		occlusion = 1;
	}

	float3 albedo = Albedo.Sample(Sampler, input.myUV).rgb;
	float3 normal = Normal.Sample(Sampler, input.myUV).rgb;

	float2 camUV = camProjection.xy;
	camUV *= 0.5;
	camUV += float2(0.5, 0.5);
	camUV.y *= -1;
	camUV.y += 1;

	const float sampleSize = 0.0004;
	const float bias = 0.0009;
	float fuzzyOcclusion = 0;

    float delta = 0.0;
	[unroll] 
	for (int i = 0; i < 64; i++)
	{
		float localDepth = Depth.Sample(Sampler, camUV + PoissonSamples[i] * sampleSize).r;
        delta += localDepth - camProjection.z;
		float localDelta = abs(camProjection.z - localDepth);
		fuzzyOcclusion += localDelta > bias;
	}

    const float threshhold = 0.2;
    if (delta > threshhold)
    {
        delta -= threshhold;
        delta *= (1 - threshhold);
        fuzzyOcclusion = lerp(fuzzyOcclusion,0, delta);
    }
	fuzzyOcclusion /= 64;

    float cloudIntensity = CloudShadow(worldPosition.xyz);
    fuzzyOcclusion = max(fuzzyOcclusion, cloudIntensity);

	//Calculate the angle value for every pixel
	float intensity = dot(normalize(DeferredFrameBuffer.environmentLightDirection.xyz), normalize(normal));
	if (intensity < 0)
		intensity = 0;


	float4 color = float4(albedo, 1) * float4(DeferredFrameBuffer.environmentLightColor.rgb, 1) * DeferredFrameBuffer.enviromentLightIntensity;
	color.a = 1;

	if (intensity > 0.4)
		color = float4(1.0, 1.0, 1.0, 1.0) * color;
	else
		color = float4(0.9, 0.9, 0.9, 1.0) * color;
    returnValue.myColor = lerp(color, color * float4(0.4, 0.4, 0.7, 1), fuzzyOcclusion);

    */
	return returnValue;
}