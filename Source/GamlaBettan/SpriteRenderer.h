#pragma once
#include "Matrix.hpp"
#include <vector>
#include "Vector.hpp"

class DirectX11Framework;
class SpriteInstance;
class Camera;
struct ID3D11DeviceContext;
struct ID3D11Buffer;

class VertexShader;
class PixelShader;

class SpriteRenderer
{
public:
	SpriteRenderer();
	~SpriteRenderer();

	bool Init(DirectX11Framework* aFramework);

	void Render(/*const Camera* aCamera,*/ const std::vector<SpriteInstance*>& aSpriteList);

private:

	struct SObjectBufferData
	{
		CommonUtilities::Matrix4x4<float> myPivotToModel;
		CommonUtilities::Matrix4x4<float> myModelToWorld;
		CommonUtilities::Vector4<float> myColor;
		CommonUtilities::Vector4<float> myUVMinMax;
		float myDepth;
		V3F trash_0;
	} myObjectBufferData;

	ID3D11DeviceContext*	myContext;
	ID3D11Buffer*			myObjectBuffer;

	VertexShader* myVertexShader;
	PixelShader* myPixelShader;
};
