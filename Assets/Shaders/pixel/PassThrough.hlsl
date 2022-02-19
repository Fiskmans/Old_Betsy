#include "../ShaderStructs.hlsli"
PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput returnValue;
	float value = AlbedoMap.Sample(defaultSampler, input.myUV[0]).r;
	//value /= 10;
	float weight = pow(abs(value), 6);
	if (value > 0.f)
	{
	returnValue.myColor.rgba = float4(
		1.0 - pow(abs(1.0 - clamp(0, 1, weight)), 4),
		clamp(0, 1, (weight - 1.0) / (weight)),
		clamp(0, 1, (weight - 50.0) / (weight)),
		1);
	}
	else
	{
		returnValue.myColor.rgba = float4(
			clamp(0, 1, (weight - 50.0) / (weight)),
			clamp(0, 1, (weight - 1.0) / (weight)),
			1.0 - pow(abs(1.0 - clamp(0, 1, weight)), 4),
			1);
	}
	return returnValue;
}