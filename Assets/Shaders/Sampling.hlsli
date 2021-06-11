

struct SampleOutput
{
	float3 albedo;
	float3 pixelNormal;
	float ambientOcclusion;
	float metalness;
	float roughness;
	float emissive;
};

SampleOutput Sample(VertexToPixel input)
{
	SampleOutput outData;

	float3 normal = float3(0.5, 0.5, 1);

#if HAS_UV_SETS
	float4 materialData = MaterialMap.Sample(defaultSampler, input.myUV).rgba;
	float4 normalMapData = NormalMap.Sample(defaultSampler, input.myUV.xy).rgba;

	normal = normalMapData.xyz;
	outData.ambientOcclusion = normalMapData.a;
	outData.metalness = materialData.r;
	outData.roughness = materialData.g;
	outData.emissive = materialData.b;

	float4 albedoData = AlbedoMap.Sample(defaultSampler, input.myUV.xy).rgba;
	outData.albedo = saturate(pow(abs(albedoData.rgb), 2.2) + tint.rgb);
#endif
#if !HAS_UV_SETS
	outData.albedo = diffuseColor;

	outData.ambientOcclusion = 0;
	outData.metalness = 0;
	outData.roughness = 0;
	outData.emissive = 0;
#endif
	
	normal = normal * 2 - 1;

	float3x3 tangentMatrix = float3x3(	normalize(input.myTangent.xyz), 
										normalize(input.myBiTangent.xyz), 
										normalize(input.myNormal.xyz));

	tangentMatrix = transpose(tangentMatrix);

	outData.pixelNormal = normalize(mul(tangentMatrix, normal.xyz));

	return outData;
}