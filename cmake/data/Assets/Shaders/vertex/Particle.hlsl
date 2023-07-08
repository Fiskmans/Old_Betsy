#include "ParticleShaderStructs.hlsli"

VertexToGeometry vertexShader(VertexInput input)
{
	VertexToGeometry  returnValue;

	float4 vertexWorldPos = mul(particleToWorldMatrix, input.myPosition.xyzw);
	returnValue.myPosition = mul(worldToCameraMatrix, vertexWorldPos);
	returnValue.myColor = input.myColor;
    
	returnValue.mySize = input.mySize;
    
    returnValue.myUVMin = input.myUVMin;
    returnValue.myUVMax = input.myUVMax;

	return returnValue;
}