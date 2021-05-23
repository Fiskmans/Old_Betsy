#include "../../ShaderStructs.hlsli"
struct GBufferOutput
{
	float4 myWorldPosition		:	SV_TARGET0;
	float4 myAlbedo				:	SV_TARGET1;
	float4 myNormal				:	SV_TARGET2;
	float4 myVertexNormal		:	SV_TARGET3;
	float myMetalness			:	SV_TARGET4;
	float myRoughness			:	SV_TARGET5;
	float myAmbientOcclusion	:	SV_TARGET6;
	float myEmissive			:	SV_TARGET7;
};
