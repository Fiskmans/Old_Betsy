float4 albedoData = AlbedoMap.Sample(defaultSampler, input.myUV.xy).rgba;
float4 materialData = MaterialMap.Sample(defaultSampler, input.myUV).rgba;
float4 normalMapData = NormalMap.Sample(defaultSampler, input.myUV.xy).rgba;

float3 albedo = saturate(pow(abs(albedoData.rgb),2.2) + tint.rgb);
float3 normal = float3(0.5,0.5,1);// normalMapData.xyz;
float ambientOcclusion = normalMapData.a;
float metalness = materialData.r;
float roughness = materialData.g;
float emissive = materialData.b;

normal = normal * 2 - 1;

float3x3 tangentMatrix = float3x3(normalize(input.myTangent.xyz), normalize(input.myBiTangent.xyz), normalize(input.myNormal.xyz));
tangentMatrix = transpose(tangentMatrix);

float3 pixelNormal = normalize(mul(tangentMatrix, normal.xyz));
//float __part = abs(cos(totalTime/2));
//
//pixelNormal = normalize(pixelNormal * __part + input.myNormal.xyz * (1 - __part));