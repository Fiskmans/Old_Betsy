#include "../../FullscreenShaderStructs.hlsli"

PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput outp;

	float3 frameColor = resource1.SampleLevel(defaultSampler, input.myUV, 1).rgb;

	//outp.myColor.rgb = frameColor;
		

	//outp.myColor.a = 1.f;

	//Version 3 sampling

	outp.myColor.rgba = float4(frameColor, 1.0f);

	/*
	float frameStepNormalized = 1 / 16;
	float colorStepNormalized = 1 / 256;

	uint blueStepMod = frac(frameColor.b * 256) * 16;
	uint blueStepFMod = fmod(frameColor.b * 256, 16);
 
	float mixF = blueStepFMod / 16;

	float blueStepNormalized = blueStepMod / 16;

	float blueStepLerp;


	if (mixF <= 0.5)

		float posBOne = LUTAutum.Sample(defaultSampler, float2(frameColor.r + blueStepNormalized, frameColor.g)).b;
		float posBOneMinus = LUTAutum.Sample(defaultSampler, float2(frameColor.r + (blueStepNormalized - frameStepNormalized), frameColor.g)).b;

		//blueStepLerp = lerp(saturate(posBOneMinus), saturate(posBOne), saturate(mixF));
		blueStepLerp = lerp(0, 1, 0);

	if (mixF > 0.5)
		
		float posBTwo = LUTAutum.Sample(defaultSampler, float2(frameColor.r + blueStepNormalized, frameColor.g)).b;
		float posBTwoPlus = LUTAutum.Sample(defaultSampler, float2(frameColor.r + (blueStepNormalized + frameStepNormalized), frameColor.g)).b;

		//blueStepLerp = lerp(saturate(posBTwo), saturate(posBTwoPlus), saturate(mixF));
		blueStepLerp = lerp(0, 1, 1);

	float3 graded = float3(blueStepLerp, blueStepLerp, blueStepLerp);
	

	//outp.myColor.rgba = float4(graded, 1.0f);
	*/

	//Version 2 sampling
	/*
	float pixelSize = 16;
	float3 color = resource11.Sample(defaultSampler, float2(pixelSize * input.myUV.x, input.myUV.y)).rgb;

	outp.myColor.rgb = color;
	outp.myColor.a = 1.f;
	*/

	return outp;
}
