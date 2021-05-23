#include "../ParticleShaderStructs.hlsli"

[maxvertexcount(4)] void geometryShader(point VertexToGeometry input[1],
	inout TriangleStream<GeometryToPixel> output)
{
	const float2 offset[4] =
	{
		{ -1.f, 1.f },
		{ 1.f, 1.f },
		{ -1.f, -1.f },
		{ 1.f, -1.f }
	};

	const float2 uv[4] =
	{
        { input[0].myUVMin.x, input[0].myUVMin.y },
		{ input[0].myUVMax.x, input[0].myUVMin.y },
		{ input[0].myUVMin.x, input[0].myUVMax.y },
		{ input[0].myUVMax.x, input[0].myUVMax.y }
	};

	VertexToGeometry  inputVertex = input[0];

	for (int i = 0; i < 4; i++)
	{
		GeometryToPixel vertex;

		vertex.myPosition = inputVertex.myPosition;
		vertex.myPosition.xy += offset[i] * inputVertex.mySize;
		vertex.myPosition = mul(cameraToProjectionMatrix, vertex.myPosition);
		vertex.myColor = inputVertex.myColor;
		vertex.myUV = uv[i];
		output.Append(vertex);
	}
}