#include "../../FullscreenShaderStructs.hlsli"


cbuffer framebuffer : register(b0)
{
	float3 cameraPosition;
	float enviromentLightIntensity;
	float3 environmentLightColor;
	float time;
	float3 environmentLightDirection;
	float cloudIntensity;
	float4x4 myToCamera;
	float4x4 myCameraProjection;
	float4x4 myBackToWorld;
	float4x4 myBackToCamera;
	float3 myCameraDirection;
	float trash_2;
};

PixelOutput pixelShader(VertexToPixel input)
{
	PixelOutput outp;

	float4 screenSpace = float4(input.myUV.x*2, -input.myUV.y*2, 0, 1);
	float4 camSpace = mul(myBackToCamera, screenSpace);
	float4 worldSpace = mul(myBackToWorld,camSpace);
	float3 groundPosition = worldSpace.xyz - myCameraDirection * (worldSpace.y / myCameraDirection.y);


	float perlin = resource1.Sample(defaultSampler, groundPosition.xz/150000 + float2(time/2000,0)).r;
	if (perlin > cloudIntensity)
	{
		discard;
	}
	outp.myColor.rgb = float3(1,1,1);
	outp.myColor.a = clamp(0,0.3, (cloudIntensity - perlin) * 2.5/cloudIntensity);

	return outp;
}