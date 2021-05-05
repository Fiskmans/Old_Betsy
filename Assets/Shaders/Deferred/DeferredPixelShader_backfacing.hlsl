#include "DeferredShaderStructs.hlsli"

float4 pixelShader(VertexToPixel input) : SV_TARGET
{
	return  input.myWorldPos;
}