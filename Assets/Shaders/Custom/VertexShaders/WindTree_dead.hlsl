#include "../../ShaderStructs.hlsli"


VertexToPixel vertexShader(VertexInput input)
{
	VertexToPixel returnValue;

#ifdef VERTEXCOLOR
    float speed = .5;
    float sinWave = sin((objectLifeTime) * speed) * sin(objectLifeTime * speed);
    float freq = 60;
    
    float myX = (input.myPosition.x) + (sinWave * freq * input.myColor.r);
    float myY = (input.myPosition.y) + (sinWave * freq * input.myColor.g);
    float myZ = (input.myPosition.z) + (sinWave * freq * input.myColor.b);
    float myW = (input.myPosition.w);
    
	//Offset for the meshes vertices
    input.myPosition = float4 (myX,myY,myZ,myW);

#else
#error
#endif

	float4x4 packedPosition = {
		input.myPosition.x,input.myPosition.y,input.myPosition.z,input.myPosition.w,
		input.myNormal.x,input.myNormal.y,input.myNormal.z,0,
		input.myTangent.x,input.myTangent.y,input.myTangent.z,0,
		input.myBiTangent.x,-input.myBiTangent.y,input.myBiTangent.z,0
	};


	float4x4 worldPosition = mul(packedPosition, modelToWorldMatrix);
	float4x4 cameraPosition = mul(worldPosition, worldToCameraMatrix);
	float4x4 screenPosition = mul(cameraPosition, cameraToProjectionMatrix);


	returnValue.myPosition = screenPosition[0];
	returnValue.myNormal = normalize(worldPosition[1]);
	returnValue.myTangent = worldPosition[2];
	returnValue.myBiTangent = worldPosition[3];
	returnValue.myUV = input.myUV;

#ifdef MULTIPLE_UV
	returnValue.myUV1 = input.myUV1;
	returnValue.myUV2 = input.myUV2;
#endif

	returnValue.myWorldPos = worldPosition[0];
#ifdef VERTEXCOLOR
	returnValue.myColor = float4(1, 0, 1, 1);
#endif


	return returnValue;
}