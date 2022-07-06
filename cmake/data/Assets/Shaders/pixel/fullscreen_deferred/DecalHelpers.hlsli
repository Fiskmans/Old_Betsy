#include "../../FullscreenShaderStructs.hlsli"
#include "../../PBRFunctions.hlsli"
struct GBufferOutput
{
    float4 myWorldPosition : SV_TARGET0;
    float4 myAlbedo : SV_TARGET1;
    float4 myNormal : SV_TARGET2;
    float4 myVertexNormal : SV_TARGET3;
    float myMetalness : SV_TARGET4;
    float myRoughness : SV_TARGET5;
    float myAmbientOcclusion : SV_TARGET6;
    float myEmissive : SV_TARGET7;
};


cbuffer LightBuffer : register(b0)
{
    float4x4 toCamera;
    float4x4 toProj;

    float4 CustomData;
    float3 Position;
    float Intensity;
    float3 CameraPosition;
    float Range;
    float LifeTime;
    float3 _;
};

TextureCube EnvironmentLight : register(t8);

Texture2D Depth : register(t15);

struct ProjectedOutput
{
    float4 myWorldPosition;
    float4 myAlbedo;
    float4 myNormal;
    float4 myVertexNormal;
    float myMetalness;
    float myRoughness;
    float myAmbientOcclusion;
    float myEmissive;
    float2 myProjectedUV;
};

ProjectedOutput Project(VertexToPixel input)
{
    ProjectedOutput output;
    
    float bias = 0.01;
    
    output.myWorldPosition = resource1.Sample(defaultSampler, input.myUV).rgba;
    output.myNormal = resource3.Sample(defaultSampler, input.myUV).rgba;

    float3 toEye = normalize(CameraPosition - output.myWorldPosition.xyz);
    float3 toLight = output.myWorldPosition.xyz - Position;

    float align = dot(normalize(toLight), output.myNormal.xyz);
    if (align > 0)
    {
        discard;
    }
	 
    float4 camPos = mul(toCamera, float4(output.myWorldPosition.xyz, 1));
    float4 projPos = mul(toProj, camPos);
    projPos /= projPos.w;
    if (projPos.x < -1 || projPos.x > 1 || projPos.y < -1 || projPos.y > 1 || projPos.z < -1 || projPos.z > 1)
    {
        discard;
    }
    output.myProjectedUV = (projPos.xy + float2(1, 1)) * float2(0.5, -0.5);
	
    float depth = resource9.Sample(defaultSampler, output.myProjectedUV).r;
    if (abs(depth - projPos.z) > bias)
    {
        discard;
    }
    
    output.myAlbedo = resource2.Sample(defaultSampler, input.myUV).rgba;
    output.myVertexNormal = resource4.Sample(defaultSampler, input.myUV);
    output.myMetalness = resource5.Sample(defaultSampler, input.myUV).r;
    output.myRoughness = resource6.Sample(defaultSampler, input.myUV).r;
    output.myAmbientOcclusion = resource7.Sample(defaultSampler, input.myUV).r;
    output.myEmissive = resource8.Sample(defaultSampler, input.myUV).r;
    
    return output;
}