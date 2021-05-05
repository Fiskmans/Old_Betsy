#pragma once
#include <Singleton.hpp>
#include <Vector3.hpp>
#include <Vector4.hpp>
#include <vector>
#include "Macros.h"
#include <Sphere.hpp>
#include <AABB3D.hpp>
#include <AABB2D.h>
#include "Matrix4x4.hpp"
#include <Plane.hpp>
#include <PlaneVolume.hpp>

class DirectX11Framework;
struct ID3D11Buffer;
struct ID3D11PixelShader;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11InputLayout;
class ModelInstance;

class DebugDrawer : public CommonUtilities::Singleton<DebugDrawer>
{
public:
	void Init(DirectX11Framework* aFramework);

	void SetColor(V4F aColor);

	void DrawLine(const V3F& aFrom, const V3F& aTo);
	void DrawLines(const std::vector<V3F>& aPoints);
	void DrawAnyLines(const V3F& aOffset, const std::vector<V3F>& aAxels, const float* aValues,const size_t& aValuesCount);
	void DrawCross(const V3F& at, const float aSize);
	void DrawArrow(const V3F& aFrom, const V3F& aTo);
	void DrawDirection(const V3F& aSource, const V3F& aDirection, const float aLength = 10);
	void DrawSphere(const CommonUtilities::Sphere<float>& aSphere, const size_t aLaps = 10);
	void DrawX(const V3F& at, const float aSize);
	void DrawY(const V3F& at, const float aSize);
	void DrawZ(const V3F& at, const float aSize);
	void DrawX(const V3F& at, const float aSize, const M44F& aSpace);
	void DrawY(const V3F& at, const float aSize, const M44F& aSpace);
	void DrawZ(const V3F& at, const float aSize, const M44F& aSpace);
	void DrawGizmo(const V3F& at, const float aSize);
	void DrawGizmo(const V3F& at, const float aSize, const M44F& aSpace);
	void DrawBoundingBox(const CommonUtilities::AABB3D<float>& aBoundingBox);
	void Draw2DBoundingBox(const CommonUtilities::AABB2D<float>& aBoundingBox);
	void DrawRotatedBoundingBox(const CommonUtilities::AABB3D<float>& aBoundingBox, const CommonUtilities::Matrix4x4<float>& aRotationMatrix);
	void DrawProgress(V3F at, float aSize, float aProgress);
	void DrawSkeleton(ModelInstance* aInstance);
	void DrawPlane(CommonUtilities::Plane<float> aPlane, float aLineSpacing = 10.f, size_t aSubdevisions = 10);
	void DrawFrustum(const CommonUtilities::PlaneVolume<float>& aFrustum);
	void DrawSpace(M44F aSpace, float aScale = 30);

	void Render(class Camera* aCamera);
private:

#ifndef _RETAIL
	unsigned int myPointCount = 0;
	const unsigned int myMaxCount = MAXDEBUGLINES;
	bool warned = false;
	bool myIsWorking = false;

	DirectX11Framework* myFramework = nullptr;
	ID3D11Buffer* myVertexBuffer = nullptr;
	ID3D11Buffer* myConstantBuffer = nullptr;
	ID3D11Buffer* myColorBuffer = nullptr;
	ID3D11PixelShader* myPixelShader = nullptr;
	ID3D11VertexShader* myVertexShader = nullptr;
	ID3D11InputLayout* myInputLayout = nullptr;

	V4F myColor;
	std::vector<V4F> myPoints;
	std::vector<std::pair<V4F, size_t>> myColorRanges;
#endif // !_RETAIL
};

