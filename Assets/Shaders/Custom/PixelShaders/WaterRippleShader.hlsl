#include "Data/Shaders/ShaderStructs.hlsli"
#include "Data/Shaders/PBRFunctions.hlsli"


PixelOutput pixelShader(VertexToPixel input)
{
#include "Data/Shaders/Sampling.hlsli"

	
	//AlbedoMap samples
	float rimWaveMaskRL = NormalMap.Sample(defaultSampler, float2(input.myUV.x, input.myUV.y - (totalTime * 0.004))).r;

	float rimWaveMaskGL = NormalMap.Sample(defaultSampler, float2(input.myUV.x, input.myUV.y - (totalTime * 0.004))).g;

	float rimWaveMaskB = NormalMap.Sample(defaultSampler, float2(input.myUV.x, input.myUV.y - (totalTime * -0.01))).b;
	float rimWaveMaskA = NormalMap.Sample(defaultSampler, float2(input.myUV.x, input.myUV.y - (totalTime * 0.01))).a;


	float rimWaveMaskGA = rimWaveMaskRL * rimWaveMaskGL * rimWaveMaskB * rimWaveMaskA;


	//float rimWaveMaskRGBAMultiplied = rimWaveMaskRGBA * waterStream;



	PixelOutput returnValuePixelOutput;
	returnValuePixelOutput.myColor = rimWaveMaskGA;

	return returnValuePixelOutput;
}