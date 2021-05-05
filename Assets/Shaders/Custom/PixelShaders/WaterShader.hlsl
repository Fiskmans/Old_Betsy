#include "Data/Shaders/ShaderStructs.hlsli"
#include "Data/Shaders/PBRFunctions.hlsli"


PixelOutput pixelShader(VertexToPixel input)
{
#include "Data/Shaders/Sampling.hlsli"

	//old color set
	//Initialize and declare color values for shallow and deep water
	float4 depthGradientShallow = float4(0.325, 0.807, 0.971, 0.725);
	float4 depthGradientDepp = float4(0.086, 0.407, 1, 0.749);
	//Initialize and declare a cap for the deepest water color

	
	//NormalMap samples start
	float rimWaveSizeU = 0.4;
	float rimWaveTimeMultiplierU = 0.003;
	float rimWaveTimeMultiplierV = 0.004;

	float rimWaveMaskRR = NormalMap.Sample(defaultSampler, float2(input.myUV.x * rimWaveSizeU - (totalTime * rimWaveTimeMultiplierU), input.myUV.y + (totalTime * rimWaveTimeMultiplierV))).r;
	float rimWaveMaskRL = NormalMap.Sample(defaultSampler, float2(input.myUV.x * rimWaveSizeU - (totalTime * rimWaveTimeMultiplierU), input.myUV.y - (totalTime * rimWaveTimeMultiplierV))).r;

	float rimWaveMaskGR = NormalMap.Sample(defaultSampler, float2(input.myUV.x * rimWaveSizeU - (totalTime * rimWaveTimeMultiplierU), input.myUV.y + (totalTime * rimWaveTimeMultiplierV))).g;
	float rimWaveMaskGL = NormalMap.Sample(defaultSampler, float2(input.myUV.x * rimWaveSizeU - (totalTime * rimWaveTimeMultiplierU), input.myUV.y - (totalTime * rimWaveTimeMultiplierV))).g;

	float rimWaveMaskB = NormalMap.Sample(defaultSampler, float2(input.myUV.x - (totalTime * 0.07), input.myUV.y)).b;
	float rimWaveMaskA = NormalMap.Sample(defaultSampler, float2(input.myUV.x - (totalTime * 0.01), input.myUV.y)).a;


	float whiteFoamAmountEdge = 1;
	float rimWaveMaskAmountR = 1;
	float rimWaveMaskAmountG = 1;
	float rimWaveMaskAmountB = 1;

	float rimWaveMaskRBRGB = rimWaveMaskRR * (rimWaveMaskGR * rimWaveMaskB);
	float rimWaveMaskGARGB = rimWaveMaskRL * (rimWaveMaskGL * rimWaveMaskA);

	float4 rimWaveMaskRB = float4 (rimWaveMaskRBRGB * rimWaveMaskAmountR, rimWaveMaskRBRGB * rimWaveMaskAmountG, rimWaveMaskRBRGB * rimWaveMaskAmountB, 1) * whiteFoamAmountEdge;
	float4 rimWaveMaskGA = float4 (rimWaveMaskGARGB * rimWaveMaskAmountR, rimWaveMaskGARGB * rimWaveMaskAmountG, rimWaveMaskGARGB * rimWaveMaskAmountB, 1) * whiteFoamAmountEdge;

	float4 rimWaveMaskRGBA = rimWaveMaskRB + rimWaveMaskGA;
	//Normalmap samples end

	//MaterialMap samples
	float waveSizeU = 0.4;
	float waveSizeV = 0.6;
	float waveTimeMultiplierURB = 0.08;
	float waveTimeMultiplierUGA = 0.0044;

	float waveMaskR = MaterialMap.Sample(defaultSampler, float2(input.myUV.x * waveSizeU - (totalTime * waveTimeMultiplierURB), input.myUV.y * waveSizeV)).r;
	float waveMaskG = MaterialMap.Sample(defaultSampler, float2(input.myUV.x * waveSizeU - (totalTime * waveTimeMultiplierUGA), input.myUV.y * waveSizeV)).g;

	float waveMaskB = MaterialMap.Sample(defaultSampler, float2(input.myUV.x * waveSizeU - (totalTime * waveTimeMultiplierURB), input.myUV.y)).b;
	float waveMaskA = MaterialMap.Sample(defaultSampler, float2(input.myUV.x * waveSizeU - (totalTime * waveTimeMultiplierUGA), input.myUV.y)).a;

	float whiteFoamAmountMiddle = 1;

	float4 waterStreamRG = float4(waveMaskR * waveMaskG, waveMaskR * waveMaskG, waveMaskR * waveMaskG, 1) * whiteFoamAmountMiddle;
	float4 waterStreamBA = float4(waveMaskB * waveMaskA, waveMaskB * waveMaskA, waveMaskB * waveMaskA, 1) * whiteFoamAmountMiddle;

	float4 waterStreamRGBA = waterStreamRG + waterStreamBA;
	//MaterialMap end


	float4 water = waterStreamRGBA + rimWaveMaskRGBA;


	//Calculate the angle value for every pixel
	float intensity = dot(normalize(environmentLightDirection.xyz), normalize(input.myNormal.xyz));
	if (intensity < 0)
		intensity = 0;

	float4 color = albedoData * float4(environmentLightColor, 1) * enviromentLightIntensity;


	if (intensity > 0)
		color = float4(1 * color.r, 1.0 * color.g, 1.0 * color.b, color.a);

	else
		color = float4(0.9 * color.r, 0.9 * color.g, 0.9 * color.b, color.a);



	PixelOutput returnValuePixelOutput;
	returnValuePixelOutput.myColor = albedoData * 0.7;
	returnValuePixelOutput.myColor += (water * 1.5) * color * 0.8;
	returnValuePixelOutput.myColor = Shade(returnValuePixelOutput.myColor, input);
	
	return returnValuePixelOutput;
}