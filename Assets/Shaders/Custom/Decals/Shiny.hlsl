#include "Data/Shaders/Deferred/DecalHelpers.hlsli"

float3 HUEtoRGB(in float H)
{
    float R = abs(H * 6 - 3) - 1;
    float G = 2 - abs(H * 6 - 2);
    float B = 2 - abs(H * 6 - 4);
    return saturate(float3(R, G, B));
}

GBufferOutput pixelShader(VertexToPixel input)
{
	GBufferOutput output;
    
    ProjectedOutput projected = Project(input);
	
    float4 spotData = resource10.Sample(defaultSampler, projected.myProjectedUV);
    float4 spotData2 = resource11.Sample(defaultSampler, projected.myProjectedUV);
	
    float shine = pow(saturate(sin(-LifeTime * 3 + projected.myProjectedUV.x + projected.myProjectedUV.y*0.5)), 7);
    float hue = sin(LifeTime * 5 - projected.myProjectedUV.x + projected.myProjectedUV.y) * 0.5 + 0.5;
    
    output.myWorldPosition = projected.myWorldPosition;
    output.myAlbedo.rgba = lerp(spotData2, float4(lerp(lerp(projected.myAlbedo.rgb, spotData.rgb, spotData.a), HUEtoRGB(hue), shine), spotData.a), sin(projected.myProjectedUV.x*4));
    output.myNormal = float4(projected.myNormal.xyz, 1);
    output.myVertexNormal = float4(projected.myVertexNormal.xyz, 1);
    output.myMetalness = 1; //metalness;
    output.myRoughness = 1; //roughness;
    output.myAmbientOcclusion = 0; // AO;
    output.myEmissive = 0; //emissive;
	
	return output;
}