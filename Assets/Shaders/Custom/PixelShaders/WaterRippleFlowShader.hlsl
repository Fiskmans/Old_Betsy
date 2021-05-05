#include "Data/Shaders/ShaderStructs.hlsli"
#include "Data/Shaders/PBRFunctions.hlsli"


PixelOutput pixelShader(VertexToPixel input)
{
#include "Data/Shaders/Sampling.hlsli"


	//NormalMap samples
	float flowWaveMaskR = NormalMap.Sample(defaultSampler, float2(input.myUV.x, input.myUV.y - (totalTime * 0.04))).r;
	float flowWaveMaskG = NormalMap.Sample(defaultSampler, float2(input.myUV.x, input.myUV.y - (totalTime * 0.15))).g;

	float flowWaveMaskB = NormalMap.Sample(defaultSampler, float2(input.myUV.x, input.myUV.y - (totalTime * 0.03))).b;
	float flowWaveMaskA = NormalMap.Sample(defaultSampler, float2(input.myUV.x, input.myUV.y - (totalTime * 0.01))).a;


	float flowWaveMaskRGBA = (flowWaveMaskR * flowWaveMaskG);

	//MaterialMap samples
	float rimWaveMaskR = MaterialMap.Sample(defaultSampler, float2(input.myUV.x - (totalTime * 0.25), input.myUV.y)).r;
	float rimWaveMaskG = MaterialMap.Sample(defaultSampler, float2(input.myUV.x + (totalTime * 0.25), input.myUV.y)).g;

	float rimWaveMaskB = MaterialMap.Sample(defaultSampler, float2(input.myUV.x, input.myUV.y - (totalTime * 0.4))).b;
	float rimWaveMaskA = MaterialMap.Sample(defaultSampler, float2(input.myUV.x, input.myUV.y + (totalTime * 0.4))).a;

	float rimWaveMaskRGBA = rimWaveMaskR * rimWaveMaskG * rimWaveMaskB * rimWaveMaskA;

	float color = flowWaveMaskRGBA * rimWaveMaskRGBA;

	PixelOutput returnValuePixelOutput;
	returnValuePixelOutput.myColor = color;

	return returnValuePixelOutput;
}