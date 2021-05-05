#include "Data/Shaders/ShaderStructs.hlsli"
SamplerState defaultSampler : register(s0);

#ifndef MULTIPLE_UV
#error "model missing data"
#endif



PixelOutput pixelShader(VertexToPixel input)
{
	//Panner for weapon slashes
	PixelOutput returnValue;


	//Divides the material channel into 3 channels for controlling the masks in different speeds
	float channelMaskR;
	channelMaskR = MaterialMap.Sample(defaultSampler, lerp(input.myUV1, input.myUV, sin(objectLifeTime) * 0.1)).r;

	float channelMaskG;
	channelMaskG = MaterialMap.Sample(defaultSampler, lerp(input.myUV1, input.myUV, sin(objectLifeTime) * 0.25)).g;

	float channelMaskB;
	channelMaskB = MaterialMap.Sample(defaultSampler, lerp(input.myUV, input.myUV1, sin(objectLifeTime))).b;


	float combined;

	if (objectLifeTime < 1)
	{
		channelMaskR = channelMaskR * (objectLifeTime)-(channelMaskB * 7);
		channelMaskG = 0;
		//channelMaskB = 0;
		//old
		//combined = (channelMaskR - objectLifeTime) + (channelMaskG - (channelMaskB *10) * objectLifeTime);
	}

	else if (objectLifeTime > 1)
	{
		//channelMaskR = (channelMaskR - (objectLifeTime - 0.6));
		channelMaskG = (channelMaskG - (objectLifeTime - 1));
		//old
		//combined = (channelMaskR - (objectLifeTime) + (channelMaskB)) + (channelMaskG * (objectLifeTime - 0.6));

	}



	//channelMaskR = channelMaskR - (objectLifeTime*0.7);
	//combined = ((channelMaskG* 2) - channelMaskB);


	returnValue.myColor = AlbedoMap.Sample(defaultSampler, input.myUV2);
	returnValue.myColor.a = channelMaskR + channelMaskG - channelMaskB;



	return returnValue;
}