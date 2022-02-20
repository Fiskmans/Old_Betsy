
#define FLT_EPSILON 1.192092896e-07f
#define nMipOffset 3


int GetNumMips(TextureCube cubeTex)
{
	int width = 0;
	int height = 0;
	int numMips = 0;

	cubeTex.GetDimensions(0, width, height, numMips);
	return numMips;
}

float RoughFromPrecp(float aRoughness)
{
	return aRoughness * aRoughness;
}

float bias(float aValue, float aBias)
{
	return (aBias > 0.0) ? pow(abs(aValue), log(aBias) / log(0.5)) : 0.0;
}

float gain(float aValue, float aGain)
{
	return 0.5 * ((aValue < 0.5) ? bias(2.0 * aValue, 1.0 - aGain) : (2.0 - bias(2.0 - 2.0 * aValue, 1.0 - aGain)));
}

float PercepRoughnessFromSpecPower(float specPower)
{
	float rough = sqrt(2.0f / (specPower + 2.0f));
	return  sqrt(max(0.f, rough));
}

float SpecPowerFromRoughness(float aRoughness)
{
	float rough = RoughFromPrecp(aRoughness);
	return (2.0f / max(FLT_EPSILON, rough * rough)) - 2.0f;
}

float BurleyToMip(float roughness, int numMips, float nDotR)
{
	float specPower = SpecPowerFromRoughness(roughness);
	specPower /= (4 * max(nDotR, FLT_EPSILON));
	float scale = PercepRoughnessFromSpecPower(specPower);

	return scale * (numMips - 1 - nMipOffset);
}

float GetReductionInMicroFacets(float aRoughness)
{
	float rough = RoughFromPrecp(aRoughness);
	return 1.f / (rough * rough + 1.f);
}

float3 GetSpecularDomDir(float3 vN, float3 vR, float realRoughness)
{
	float invRealRough = saturate(1 - realRoughness);
	float lerpFactor = invRealRough * (sqrt(invRealRough) + realRoughness);

	return lerp(vN, vR, lerpFactor);
}

float EmpSpecAO(float ao, float aRoughness)
{
	float smooth = 1 - aRoughness;
	float specAo = gain(ao, 0.5f + max(0.f, smooth * 0.4f));

	return min(1.f, specAo + lerp(0.f, 0.5f, smooth * smooth * smooth * smooth));
}

float ApproxSpecSelfOcc(float3 aReflection, float3 aNormal)
{
	const float fadeParam = 1.3f;
	float rimmask = clamp(1.0f + fadeParam * dot(aReflection, aNormal), 0.0, 1.0);
	rimmask *= rimmask;
	return rimmask;
}

float3 EvaluateAmbience(TextureCube lysBurleyCube, float3 vN, float3 orgNormal, float3 toCam, float roughness, float metalness, float3 albedo, float ao, float3 diffColor, float3 specColor)
{
	int numMips = GetNumMips(lysBurleyCube);
	const int nrBrdfMips = numMips - nMipOffset;
	float vDotN = clamp(dot(toCam, vN), 0.0f, 1.0f);
	const float3 orgReflect = 2 * vN * vDotN - toCam;

	float3 vR = GetSpecularDomDir(vN, orgReflect, RoughFromPrecp(roughness));
	float rDotN = saturate(dot(vN, vR));

	float level = BurleyToMip(roughness, numMips, rDotN);

	float3 specRad = lysBurleyCube.SampleLevel(defaultSampler, vR, level).xyz;
	float3 diffRad = lysBurleyCube.SampleLevel(defaultSampler, vN, (float)(nrBrdfMips - 1)).xyz;

	float ft = 1.0f - rDotN;
	float ft5 = ft * ft;
	ft5 = ft5 * ft5 * ft;
	specColor = lerp(specColor, (float3)1.f, ft5);

	float fade = GetReductionInMicroFacets(roughness);
	fade *= EmpSpecAO(ao, roughness);
	fade *= ApproxSpecSelfOcc(vR, orgNormal);

	float3 ambientDiff = ao * diffColor * diffRad;
	float3 ambientSpec = fade * specColor * specRad;

	return ambientDiff + ambientSpec;

}

static const float PI = 3.141596f;
float3 Diffuse(float3 aAlbedo)
{
	return aAlbedo / PI;
}

float NormalDistribution_GGX(float a, float NdH)
{
	float a2 = a * a;
	float NdH2 = NdH * NdH;

	float denom = NdH2 * (a2 - 1.0f) + 1.0f;
	denom *= denom;
	denom *= PI;
	return a2 / denom;
}

float Specular_D(float a, float NdH)
{
	return NormalDistribution_GGX(a, NdH);
}

float Geometric_Smith_Schlick_GGX(float a, float NdV, float NdL)
{
	float k = a * 0.5f;
	float GV = NdV / (NdV * (1 - k) + k);
	float GL = NdL / (NdL * (1 - k) + k);
	return GV * GL;
}

float Specular_G(float a, float NdV, float NdL, float NdH, float VdH, float LdV)
{
	return Geometric_Smith_Schlick_GGX(a, NdV, NdL);
}


float3 Fresnel_Schlick(float3 specularColor, float VdH)
{
	return (specularColor + (1.0f - specularColor) * pow((1.0f - VdH), 5));
}

float3 Specular_F(float3 specularColor, float VdH)
{
	return Fresnel_Schlick(specularColor, VdH);
}

float3 Specular(float3 aSpecularColor, float3 aH, float3 aV, float3 aL, float aA, float aNdL, float aNdV, float aNdH, float aVdH, float aLdV)
{
	return ((Specular_D(aA, aNdH) * Specular_G(aA, aNdV, aNdL, aNdH, aVdH, aLdV)) * Specular_F(aSpecularColor, aVdH)) / (4.0f * aNdL * aNdV + 0.0001f);
}

float3 EvaluateDirectionalLight(float3 aAlbedoColor, float3 aSpecularColor, float3 aNormal, float aRoughness, float aLightIntensity, float3 aLightColor, float3 aLightDirection, float3 aViewDirection)
{
	float3 h = normalize(aLightDirection + aViewDirection);
	float a = max(0.001f, RoughFromPrecp(aRoughness));

	float lambert = saturate(dot(aNormal, aLightDirection));
	float normalDotView = saturate(dot(aNormal, aViewDirection));
	float normalDotH = saturate(dot(aNormal, h));
	float viewDotH = saturate(dot(aViewDirection, h));
	float lightDotView = saturate(dot(aLightDirection, aViewDirection));

	float3 cDiff = Diffuse(aAlbedoColor);
	float3 cSpec = Specular(aSpecularColor, h, aViewDirection, aLightDirection, a, lambert, normalDotView, normalDotH, viewDotH, lightDotView);

	return saturate(aLightColor * aLightIntensity * lambert * (cDiff * (1.0f - cSpec) + cSpec) * PI);
}


float3 EvaluatePointLight(float3 aAlbedoColor, float3 aSpecularColor, float3 aNormal, float aRoughness, float3 aLightColor, float aLightIntensity, float aRange, float3 aPosition, float3 aPixelPosition, float3 aViewDirection)
{
	float3 toLight = aPosition.xyz - aPixelPosition.xyz;
	float lightDist = length(toLight);
	toLight = normalize(toLight);

	float3 h = normalize(toLight + aViewDirection);
	float a = max(0.001f, RoughFromPrecp(aRoughness));

	float lambert = saturate(dot(aNormal, toLight));
	float normalDotView = saturate(dot(aNormal, aViewDirection));
	float normalDotH = saturate(dot(aNormal, h));
	float viewDotH = saturate(dot(aViewDirection, h));
	float lightDotView = saturate(dot(toLight, aViewDirection));

	float3 cDiff = Diffuse(aAlbedoColor);
	float3 cSpec = Specular(aSpecularColor, h, aViewDirection, toLight, a, lambert, normalDotView, normalDotH, viewDotH, lightDotView);

	float linAtten = saturate(1 - lightDist / aRange);
	float physAtten = saturate(1 / (lightDist * lightDist));
	float atten = lambert * linAtten * physAtten;

	return saturate(aLightColor * atten * aLightIntensity * ((cDiff * (1.0f - cSpec) + cSpec) * PI));
}

float4 EvaluatePointLight_Alpha(float3 aAlbedoColor, float3 aSpecularColor, float3 aNormal, float aRoughness, float3 aLightColor, float aLightIntensity, float aRange, float3 aPosition, float3 aPixelPosition, float3 aViewDirection)
{
	float3 toLight = aPosition.xyz - aPixelPosition.xyz;
	float lightDist = length(toLight);
	toLight = normalize(toLight);

	float3 h = normalize(toLight + aViewDirection);
	float a = max(0.001f, RoughFromPrecp(aRoughness));

	float lambert = saturate(dot(aNormal, toLight));
	float normalDotView = saturate(dot(aNormal, aViewDirection));
	float normalDotH = saturate(dot(aNormal, h));
	float viewDotH = saturate(dot(aViewDirection, h));
	float lightDotView = saturate(dot(toLight, aViewDirection));

	float3 cDiff = Diffuse(aAlbedoColor);
	float3 cSpec = Specular(aSpecularColor, h, aViewDirection, toLight, a, lambert, normalDotView, normalDotH, viewDotH, lightDotView);

	float linAtten = saturate(1 - lightDist / aRange);
	float physAtten = saturate(1 / (lightDist * lightDist));
	float atten = lambert * linAtten * physAtten;
	//return float4(float3(atten, atten, atten) * aLightColor,1);

	return float4(saturate(aLightColor  * ((cDiff * (1.0f - cSpec) + cSpec) * PI)),atten * aLightIntensity);
}