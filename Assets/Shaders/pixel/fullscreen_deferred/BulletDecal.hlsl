#include "DecalHelpers.hlsli"


GBufferOutput pixelShader(VertexToPixel input)
{
	GBufferOutput output;
    
    ProjectedOutput projected = Project(input);
	
    float4 spotData = resource10.Sample(defaultSampler, projected.myProjectedUV);
    float4 spotData2 = resource11.Sample(defaultSampler, projected.myProjectedUV);
    float4 spotData3 = resource12.Sample(defaultSampler, projected.myProjectedUV);

    output.myWorldPosition = projected.myWorldPosition;
    output.myAlbedo.rgba = float4(lerp(projected.myAlbedo.rgb, spotData.rgb, spotData.a), spotData.a);

    float3 n1 = projected.myVertexNormal.xyz * 2 - 1;
    float3 n2 = spotData3.xyz * 2 - 1;

    //float3 r = normalize(float3(n1.xy*n2.z + n2.xy*n1.z, n1.z*n2.z));
    float3 r = normalize(float3(n1.xy + n2.xy, n1.z*n2.z));
    //float3 r = normalize(float3(n1.xy + n2.xy, n1.z));

    float3 normaBlend = r*0.5 + 0.5;

    output.myNormal = float4(normaBlend, spotData.a);
    output.myVertexNormal = float4(normaBlend, spotData.a);


    //output.myNormal = float4(projected.myVertexNormal.xyz, spotData.a);
    //output.myVertexNormal = float4(projected.myVertexNormal.xyz, spotData.a);

    output.myMetalness = spotData2.r; //metalness;
    output.myRoughness = spotData2.g; //roughness;
    output.myAmbientOcclusion = spotData2.b; // AO;
    output.myEmissive = spotData2.a; //emissive;
	
	return output;
}