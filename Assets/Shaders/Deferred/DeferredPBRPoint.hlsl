#include "../Fullscreen/FullscreenShaderStructs.hlsli"
#include "../PBRFunctions.hlsli"

cbuffer LightBuffer : register(b0)
{
	float4x4 toCamera[6];
	float4x4 toProj[6];

	float3 LightColor;
	float Intensity;
	float3 Position;
	float Range;
	float3 CameraPosition;
	float trash;
};

TextureCube EnvironmentLight : register(t8);

Texture2D Depth : register(t15);

unsigned int MajorAxis(float3 aVector)
{
	float x = abs(aVector.x);
	float y = abs(aVector.y);
	float z = abs(aVector.z);


	unsigned int axisIndex = 2 * (y > x&& y > z) + 4 * (z > x&& z > y);
	axisIndex += (axisIndex == 0 && aVector.x < 0);
	axisIndex += (axisIndex == 2 && aVector.y < 0);
	axisIndex += (axisIndex == 4 && aVector.z < 0);
	return axisIndex;
}

PixelOutput pixelShader(VertexToPixel input)
{
	float bias = 0.001;
	if (Depth.Sample(defaultSampler, input.myUV).r == 1)
	{
		discard;
	}

	PixelOutput returnValue;
	float3 worldPosition = resource1.Sample(defaultSampler, input.myUV).rgb;
	float3 albedo = resource2.Sample(defaultSampler, input.myUV).rgb;
	float3 normal = resource3.Sample(defaultSampler, input.myUV).rgb;
	//float3 worldNormal = resource4.Sample(defaultSampler, input.myUV).rgb;
	float metalness = resource5.Sample(defaultSampler, input.myUV).r;
	float roughness = resource6.Sample(defaultSampler, input.myUV).r;
	//float ambientOcclusion = resource7.Sample(defaultSampler, input.myUV).r;
	//float emissive = resource8.Sample(defaultSampler, input.myUV).r;

	float3 toEye = normalize(CameraPosition - worldPosition);

	float3 specColor = lerp((float3) 0.04, albedo, metalness);
	float3 diffColor = lerp((float3) 0.00, albedo, 1 - metalness);

	


	float3 toLight = worldPosition - Position;



	unsigned int cameraIndex = MajorAxis(toLight);

	unsigned int reMapping[6] = 
	{
		5,
		4,
		2,
		3,
		0,
		1
	};
	cameraIndex = reMapping[cameraIndex];
	 
	//worldPosition.z *= -1;
	float4 camPos = mul(toCamera[cameraIndex], float4(worldPosition, 1));
	float4 projPos = mul(toProj[cameraIndex],camPos);
	projPos /= projPos.w;
	if (projPos.x < -1 || projPos.x > 1 || projPos.y < -1 || projPos.y > 1 || projPos.z < -1 || projPos.z > 1)
	{
		discard;
	}
	
    float2 camOffset = float2(1.0 / 6.0 * (cameraIndex +1), 0);
    float2 projuv = (projPos.xy - float2(1, -1)) * float2(0.5 / 6, -0.5) + camOffset + float2(0,1);
	

	float depth = resource9.Sample(defaultSampler, projuv).r;

	if (depth < projPos.z - bias)
	{
		discard;
	}

	float sampleRad = 0.0003;
	
	float occlusion = 0;
    const float2 offsets[] = { float2(1, 0), float2(-1, 0), float2(0, 1), float2(0, -1),
    float2(0.7, 0.7), float2(0.7, 0.7), float2(0.7, 0.7), float2(0.7, 0.7)
    };
    const int offsetCount = 8;
	
	
    returnValue.myColor.rg = float2(1,1);
    [unroll] 
    for (int i = 0; i < offsetCount; i++)
    {
        float2 offset = projuv + offsets[i] * sampleRad;
        float2 samppos = float2(clamp(offset.x, 1.0 / 6.0 * cameraIndex, 1.0 / 6.0 * (cameraIndex+1)), saturate(offset.y));
		
        occlusion += resource9.Sample(defaultSampler, samppos).r > depth - bias;
    }
    occlusion /= offsetCount;
	
    returnValue.myColor.rgba = pow(abs(EvaluatePointLight_Alpha(diffColor, specColor, normal, roughness, LightColor, Intensity, Range, Position, worldPosition, toEye)), 1.0 / 2.2) * occlusion;

	return returnValue;
}