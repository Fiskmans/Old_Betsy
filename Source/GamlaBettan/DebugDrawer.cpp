#include "pch.h"
#include "DebugDrawer.h"
#include <d3d11.h>
#include "DirectX11Framework.h"
#include <Matrix4x4.hpp>
#include "ModelLoader.h"
#include "Camera.h"
#include "ShaderCompiler.h"
#include "ModelInstance.h"
#include "Model.h"
#include "ShaderFlags.h"
#include "Intersection.hpp"

struct CameraBuffer
{
	CommonUtilities::Matrix4x4<float> myToCamera;
	CommonUtilities::Matrix4x4<float> myToProjection;
};

void DebugDrawer::Init(DirectX11Framework* aFramework)
{
#if USEDEBUGLINES
	myFramework = aFramework;
	ID3D11Device* device = aFramework->GetDevice();

	std::string pixelShaderData =
		"cbuffer ColorBuffer : register(b0)"
		"{"
		"	float4 color : COLOR;"
		"};"
		"float4 pixelShader(float4 position : SV_POSITION) : SV_TARGET\n"
		"{\n"
		"	return color;\n"
		"}\n"
		;

	if (!CompilePixelShader(device, pixelShaderData, myPixelShader))
	{
		SYSERROR("Could not compile pixelshader", "debugdrawer");
		return;
	}

	std::string vertexShaderData =
		"cbuffer matrixes : register(b0)\n"
		"{\n"
		"	float4x4 toCamera;\n"
		"	float4x4 toProjection;\n"
		"};\n"
		"struct inputStruct\n"
		"{\n"
		"	float4 position : POSITION; \n"
		"};\n"
		"float4 vertexShader(inputStruct pos) : SV_POSITION\n"
		"{\n"
		"	float4 camerapos = mul(pos.position,toCamera);\n"
		"	return mul(camerapos,toProjection);\n"
		"}\n"
		;

	ID3DBlob* vertexBlob;
	if (!CompileVertexShader(device, vertexShaderData, myVertexShader, &vertexBlob))
	{
		if (vertexBlob)
		{
			vertexBlob->Release();
		}
		SYSERROR("Could not Compile vertexshader", "debugdrawer");
		return;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION" ,0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	HRESULT result = device->CreateInputLayout(layout, sizeof(layout) / sizeof(layout[0]), vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &myInputLayout);
	if (FAILED(result))
	{
		SYSERROR("could not create input layout", "debugdrawer");
		return;
	}

	CD3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = sizeof(V4F) * myMaxCount;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(V4F);

	result = device->CreateBuffer(&desc, nullptr, &myVertexBuffer);
	if (FAILED(result))
	{
		SYSERROR("could not create vertex buffer", "debugdrawer");
		return;
	}

	desc.ByteWidth = sizeof(CameraBuffer);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.StructureByteStride = 0;

	result = device->CreateBuffer(&desc, nullptr, &myConstantBuffer);
	if (FAILED(result))
	{
		SYSERROR("could not create constant buffer", "debugdrawer");
		return;
	}

	desc.ByteWidth = sizeof(V4F);
	desc.StructureByteStride = 0;

	result = device->CreateBuffer(&desc, nullptr, &myColorBuffer);
	if (FAILED(result))
	{
		SYSERROR("could not create Color buffer", "debugdrawer");
		return;
	}


	myIsWorking = true;
#endif // !_RETAIL
}

void DebugDrawer::SetColor(V4F aColor)
{
#if USEDEBUGLINES
	if (myColor != aColor)
	{
		myColorRanges.emplace_back(myColor, myPoints.size());
		myColor = aColor;
	}
#endif
}

void DebugDrawer::DrawLine(const V3F& aFrom, const V3F& aTo)
{
#if USEDEBUGLINES
	if (myIsWorking)
	{
		if (myPointCount + 2 < myMaxCount)
		{
			myPoints.push_back(V4F(aFrom, 1));
			myPoints.push_back(V4F(aTo, 1));
			myPointCount += 2;
		}
		else
		{
			if (!warned)
			{
				SYSWARNING(std::string() + "Drawing a lot of debug lines consider reducing amount: " + STRINGVALUE(MAXDEBUGLINES), "");
				warned = true;
			}
		}
	}
#endif // !_RETAIL
}


void DebugDrawer::DrawLines(const std::vector<V3F>& aPoints)
{
#if USEDEBUGLINES
	for (size_t i = 1; i < aPoints.size(); i++)
	{
		DrawLine(aPoints[i - 1], aPoints[i]);
	}
#endif
}

void DebugDrawer::DrawAnyLines(const V3F& aOffset, const std::vector<V3F>& aAxels, const float* aValues, const size_t& aValuesCount)
{
#if USEDEBUGLINES
	std::vector<V3F> translatedPoints;
	const float* at = aValues;
	for (size_t i = 0; i < aValuesCount; i++)
	{
		V3F pos = aOffset;
		for (auto& axel : aAxels)
		{
			pos += axel * *(at++);
		}
		translatedPoints.push_back(pos);
	}
	DrawLines(translatedPoints);
#endif // USEDEBUGLINES
}

void DebugDrawer::DrawCross(const V3F& at, const float aSize)
{
#if USEDEBUGLINES
	DrawLine(at + V3F(-aSize, -aSize, -aSize), at + V3F(aSize, aSize, aSize));
	DrawLine(at + V3F(-aSize, -aSize, aSize), at + V3F(aSize, aSize, -aSize));
	DrawLine(at + V3F(-aSize, aSize, -aSize), at + V3F(aSize, -aSize, aSize));
	DrawLine(at + V3F(-aSize, aSize, aSize), at + V3F(aSize, -aSize, -aSize));
#endif
}

void DebugDrawer::DrawArrow(const V3F& aFrom, const V3F& aTo)
{
#if USEDEBUGLINES
	DrawLine(aFrom, aTo);

	V3F worldUp = { 0,1,0 };
	V3F forward = (aTo - aFrom);
	V3F right = { 1,0,0 };
	if (forward.x != 0.f || forward.z != 0.f)
	{
		right = forward.Cross(worldUp);
	}
	V3F up = right.Cross(forward);
	right.Normalize();
	up.Normalize();

	DrawLine(aTo, aFrom + forward * 0.6f + right * forward.Length() * 0.16f);
	DrawLine(aTo, aFrom + forward * 0.6f - right * forward.Length() * 0.16f);

	DrawLine(aTo, aFrom + forward * 0.6f + up * forward.Length() * 0.16f);
	DrawLine(aTo, aFrom + forward * 0.6f - up * forward.Length() * 0.16f);
#endif
}

void DebugDrawer::DrawDirection(const V3F& aSource, const V3F& aDirection, const float aLength)
{
#if USEDEBUGLINES
	DrawArrow(aSource, aSource + aDirection.GetNormalized() * aLength);
#endif
}

void DebugDrawer::DrawSphere(const CommonUtilities::Sphere<float>& aSphere, const size_t aLaps)
{
#if USEDEBUGLINES
	const int linesperlap = 20;
	V3F last = aSphere.Position() - V3F(0, -1, 0) * aSphere.Radius();
	for (size_t i = 0; i < aLaps * linesperlap; i++)
	{
		float l = i / static_cast<float>(aLaps * linesperlap)* PI;
		float y = cos(l);
		float r = sin(l);

		V3F next = V3F(sin(i * PI / linesperlap) * r, y, cos(i * PI / linesperlap) * r) * aSphere.Radius() + aSphere.Position();
		DrawLine(last, next);
		last = next;
	}
#endif
}

void DebugDrawer::DrawX(const V3F& at, const float aSize)
{
#if USEDEBUGLINES
	DrawLine(at + V3F(0, aSize, aSize), at - V3F(0, aSize, aSize));
	DrawLine(at + V3F(0, -aSize, aSize), at - V3F(0, -aSize, aSize));
#endif
}

void DebugDrawer::DrawY(const V3F& at, const float aSize)
{
#if USEDEBUGLINES
	DrawLine(at, at + V3F(aSize, aSize, 0));
	DrawLine(at, at + V3F(-aSize, aSize, 0));
	DrawLine(at, at + V3F(0, -aSize, 0));
#endif
}

void DebugDrawer::DrawZ(const V3F& at, const float aSize)
{
#if USEDEBUGLINES
	DrawLine(at + V3F(-aSize, aSize, 0), at + V3F(aSize, aSize, 0));
	DrawLine(at + V3F(-aSize, -aSize, 0), at + V3F(aSize, aSize, 0));
	DrawLine(at + V3F(-aSize, -aSize, 0), at + V3F(aSize, -aSize, 0));
#endif
}

void DebugDrawer::DrawX(const V3F& at, const float aSize, const M44F& aSpace)
{
#if USEDEBUGLINES
	DrawLine(at + V3F(0, aSize, aSize), at + V4F(0, -aSize, -aSize, 0) * aSpace);
	DrawLine(at + V3F(0, -aSize, aSize), at + V4F(0, aSize, -aSize, 0) * aSpace);
#endif
}

void DebugDrawer::DrawY(const V3F& at, const float aSize, const M44F& aSpace)
{
#if USEDEBUGLINES
	DrawLine(V4F(at, 1), at + V4F(aSize, aSize, 0, 0) * aSpace);
	DrawLine(V4F(at, 1), at + V4F(-aSize, aSize, 0, 0) * aSpace);
	DrawLine(V4F(at, 1), at + V4F(0, -aSize, 0, 0) * aSpace);
#endif
}

void DebugDrawer::DrawZ(const V3F& at, const float aSize, const M44F& aSpace)
{
#if USEDEBUGLINES
	DrawLine(at + V3F(-aSize, aSize, 0), at + V4F(aSize, aSize, 0, 0) * aSpace);
	DrawLine(at + V3F(-aSize, -aSize, 0), at + V4F(aSize, aSize, 0, 0) * aSpace);
	DrawLine(at + V3F(-aSize, -aSize, 0), at + V4F(aSize, -aSize, 0, 0) * aSpace);
#endif
}

void DebugDrawer::DrawGizmo(const V3F& at, const float aSize)
{
#if USEDEBUGLINES
	DebugDrawer::GetInstance().DrawDirection(at, V3F(1, 0, 0), aSize);
	DebugDrawer::GetInstance().DrawDirection(at, V3F(0, 1, 0), aSize);
	DebugDrawer::GetInstance().DrawDirection(at, V3F(0, 0, 1), aSize);
	DebugDrawer::GetInstance().DrawX(at + V3F(1, 0, 0) * aSize * 1.3f, aSize * 0.2f);
	DebugDrawer::GetInstance().DrawY(at + V3F(0, 1, 0) * aSize * 1.3f, aSize * 0.2f);
	DebugDrawer::GetInstance().DrawZ(at + V3F(0, 0, 1) * aSize * 1.3f, aSize * 0.2f);
#endif
}

void DebugDrawer::DrawGizmo(const V3F& at, const float aSize, const M44F& aSpace)
{
#if USEDEBUGLINES
	SetColor(V4F(1, 0, 0, 1));
	DebugDrawer::GetInstance().DrawDirection(at, aSpace.Column(0), aSize);
	DebugDrawer::GetInstance().DrawX(at + aSpace.Column(0) * aSize * 1.3f, aSize * 0.2f, aSpace);

	SetColor(V4F(0, 1, 0, 1));
	DebugDrawer::GetInstance().DrawY(at + aSpace.Column(1) * aSize * 1.3f, aSize * 0.2f, aSpace);
	DebugDrawer::GetInstance().DrawDirection(at, aSpace.Column(1), aSize);

	SetColor(V4F(0, 0, 1, 1));
	DebugDrawer::GetInstance().DrawZ(at + aSpace.Column(2) * aSize * 1.3f, aSize * 0.2f, aSpace);
	DebugDrawer::GetInstance().DrawDirection(at, aSpace.Column(2), aSize);
#endif
}

void DebugDrawer::DrawBoundingBox(const CommonUtilities::AABB3D<float>& aBoundingBox)
{
#if USEDEBUGLINES
	V3F iii = aBoundingBox.Min();
	V3F aaa = aBoundingBox.Max();

	V3F iaa = V3F(iii.x, aaa.y, aaa.z);
	V3F aia = V3F(aaa.x, iii.y, aaa.z);
	V3F iia = V3F(iii.x, iii.y, aaa.z);
	V3F aai = V3F(aaa.x, aaa.y, iii.z);
	V3F iai = V3F(iii.x, aaa.y, iii.z);
	V3F aii = V3F(aaa.x, iii.y, iii.z);

	DrawLine(iii, aii);
	DrawLine(iai, aai);
	DrawLine(iia, aia);
	DrawLine(iaa, aaa);

	DrawLine(iii, iai);
	DrawLine(aii, aai);
	DrawLine(iia, iaa);
	DrawLine(aia, aaa);

	DrawLine(iii, iia);
	DrawLine(aii, aia);
	DrawLine(iai, iaa);
	DrawLine(aai, aaa);

#endif
}

void DebugDrawer::DrawRotatedBoundingBox(const CommonUtilities::AABB3D<float>& aBoundingBox, const CommonUtilities::Matrix4x4<float>& aTransformMatrix)
{
#if USEDEBUGLINES
	V3F iii = aBoundingBox.Min();
	V3F aaa = aBoundingBox.Max();

	V4F iii4 = V4F(iii.x, iii.y, iii.z, 1.0f);
	V4F aaa4 = V4F(aaa.x, aaa.y, aaa.z, 1.0f);

	V4F iaa = V4F(iii.x, aaa.y, aaa.z, 1.0f);
	V4F aia = V4F(aaa.x, iii.y, aaa.z, 1.0f);
	V4F iia = V4F(iii.x, iii.y, aaa.z, 1.0f);
	V4F aai = V4F(aaa.x, aaa.y, iii.z, 1.0f);
	V4F iai = V4F(iii.x, aaa.y, iii.z, 1.0f);
	V4F aii = V4F(aaa.x, iii.y, iii.z, 1.0f);

	iii4 = iii4 * aTransformMatrix;
	aaa4 = aaa4 * aTransformMatrix;

	iaa = iaa * aTransformMatrix;
	aia = aia * aTransformMatrix;
	iia = iia * aTransformMatrix;
	aai = aai * aTransformMatrix;
	iai = iai * aTransformMatrix;
	aii = aii * aTransformMatrix;

	DrawLine(V3F(iii4), V3F(aii));
	DrawLine(iai, aai);
	DrawLine(iia, aia);
	DrawLine(iaa, aaa4);

	DrawLine(iii4, iai);
	DrawLine(aii, aai);
	DrawLine(iia, iaa);
	DrawLine(aia, aaa4);

	DrawLine(iii4, iia);
	DrawLine(aii, aia);
	DrawLine(iai, iaa);
	DrawLine(aai, aaa4);
#endif
}

void DebugDrawer::Draw2DBoundingBox(const CommonUtilities::AABB2D<float>& aBoundingBox)
{
#if USEDEBUGLINES && USEIMGUI
	V2F iii = V2F(aBoundingBox.Min()) * V2F(1920.f, 1080.f);
	V2F aaa = V2F(aBoundingBox.Max()) * V2F(1920.f, 1080.f);

	V2F iaa = V2F(iii.x, aaa.y);
	V2F aia = V2F(aaa.x, iii.y);
	V2F iia = V2F(iii.x, iii.y);
	V2F aai = V2F(aaa.x, aaa.y);

	auto color = IM_COL32(myColor.x * 225, myColor.y * 225, myColor.z * 225, myColor.w * 225);

	ImGui::GetForegroundDrawList()->AddRect(ImVec2(iia.x, iia.y), ImVec2(aia.x, aia.y), color);
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(iaa.x, iaa.y), ImVec2(aaa.x, aaa.y), color);

	ImGui::GetForegroundDrawList()->AddRect(ImVec2(iia.x, iia.y), ImVec2(iaa.x, iaa.y), color);
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(aia.x, aia.y), ImVec2(aaa.x, aaa.y), color);

	ImGui::GetForegroundDrawList()->AddRect(ImVec2(iii.x, iii.y), ImVec2(iia.x, iia.y), color);
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(aai.x, aai.y), ImVec2(aaa.x, aaa.y), color);
#endif
}

void DebugDrawer::DrawProgress(V3F at, float aSize, float aProgress)
{
#if USEDEBUGLINES
	V3F right = V3F(1, 0, 0);
	V3F up = V3F(0, 1, 0);
	DrawLine(at - right * aSize + up * aSize * 0.1f, at - right * aSize - up * aSize * 0.1f);
	DrawLine(at + right * aSize + up * aSize * 0.1f, at + right * aSize - up * aSize * 0.1f);

	DrawLine(at - right * aSize, at + right * aSize * (aProgress * 2 - 1));
#endif
}

void DebugDrawer::DrawSkeleton(ModelInstance* aInstance)
{
#if USEDEBUGLINES
	SetColor(V4F(0, 1, 0, 1));
	if (aInstance->GetModelAsset().GetAsModel()->GetModelData()->myshaderTypeFlags & ShaderFlags::HasBones)
	{

		auto positions = aInstance->GetBonePositions();

		for (size_t i = 0; i < aInstance->GetModelAsset().GetAsModel()->myBoneData.size(); i++)
		{
			if (aInstance->GetModelAsset().GetAsModel()->myBoneData[i].parent != -1)
			{
				DrawLine(positions[aInstance->GetModelAsset().GetAsModel()->myBoneData[i].parent], positions[i]);
			}
			else
			{
				DrawLine(aInstance->GetPosition(), positions[i]);
			}
			//DrawSphere(CommonUtilities::Sphere<float>(positions[i], 5), 8);

		}
	}
#endif
}

void DebugDrawer::DrawPlane(CommonUtilities::Plane<float> aPlane, float aLineSpacing, size_t aSubdevisions)
{
#if USEDEBUGLINES
	V3F right = aPlane.Normal().Cross(V3F(0, 1, 0));
	V3F up = right.Cross(aPlane.Normal());
	right.Normalize();
	up.Normalize();
	right *= aLineSpacing;
	up *= aLineSpacing;

	V3F center = aPlane.Point();
	float negativity = -float(aSubdevisions) / 2;

	for (size_t x = 0; x < aSubdevisions + 1; x++)
	{
		for (size_t y = 0; y < aSubdevisions + 1; y++)
		{
			DrawLine(right * negativity + up * (y + negativity) + center, right * -negativity + up * (y + negativity) + center);
			DrawLine(right * (x + negativity) + up * negativity + center, right * (x + negativity) + up * -negativity + center);
		}
	}
#endif
}

void DebugDrawer::DrawFrustum(const CommonUtilities::PlaneVolume<float>& aFrustum)
{
#if USEDEBUGLINES
	const std::vector<CommonUtilities::Plane<float>>& planes = aFrustum.Planes();
	if (planes.size() == 6)
	{
		static int iterations = 120;
		V3F nearTopRight = CommonUtilities::SolvePlaneIntersection<float>(planes[0], planes[2], planes[5], iterations);
		V3F nearBottomRight = CommonUtilities::SolvePlaneIntersection<float>(planes[0], planes[3], planes[5], iterations);

		V3F nearTopLeft = CommonUtilities::SolvePlaneIntersection<float>(planes[1], planes[2], planes[5], iterations);
		V3F nearBottomLeft = CommonUtilities::SolvePlaneIntersection<float>(planes[1], planes[3], planes[5], iterations);

		V3F farTopRight = CommonUtilities::SolvePlaneIntersection<float>(planes[0], planes[2], planes[4], iterations);
		V3F farBottomRight = CommonUtilities::SolvePlaneIntersection<float>(planes[0], planes[3], planes[4], iterations);

		V3F farTopLeft = CommonUtilities::SolvePlaneIntersection<float>(planes[1], planes[2], planes[4], iterations);
		V3F farBottomLeft = CommonUtilities::SolvePlaneIntersection<float>(planes[1], planes[3], planes[4], iterations);

		for (size_t i = 0; i < 6; i++)
		{
			float part = float(i) / 5.f;

			//near
			DrawArrow(LERP(nearBottomRight, nearBottomLeft, part), LERP(nearTopRight, nearTopLeft, part));
			DrawLine(LERP(nearBottomLeft, nearTopLeft, part), LERP(nearBottomRight, nearTopRight, part));

			//left
			DrawLine(LERP(nearBottomLeft, nearTopLeft, part), LERP(farBottomLeft, farTopLeft, part));
			DrawLine(LERP(nearBottomLeft, farBottomLeft, part), LERP(nearTopLeft, farTopLeft, part));

			//right
			DrawLine(LERP(nearBottomRight, nearTopRight, part), LERP(farBottomRight, farTopRight, part));
			DrawLine(LERP(nearBottomRight, farBottomRight, part), LERP(nearTopRight, farTopRight, part));

			//Top
			DrawLine(LERP(nearTopLeft, nearTopRight, part), LERP(farTopLeft, farTopRight, part));
			DrawLine(LERP(nearTopRight, farTopRight, part), LERP(nearTopLeft, farTopLeft, part));

			//Bottom
			DrawLine(LERP(nearBottomLeft, nearBottomRight, part), LERP(farBottomLeft, farBottomRight, part));
			DrawLine(LERP(nearBottomRight, farBottomRight, part), LERP(nearBottomLeft, farBottomLeft, part));

			//Far
			DrawArrow(LERP(farBottomRight, farBottomLeft, part), LERP(farTopRight, farTopLeft, part));
			DrawLine(LERP(farBottomLeft, farTopLeft, part), LERP(farBottomRight, farTopRight, part));
		}

		for (auto& i : aFrustum.Planes())
		{
			DrawDirection(i.Point(), i.Normal(), 5);
		}
	}
#endif
}

void DebugDrawer::DrawSpace(M44F aSpace, float aScale)
{
#if USEDEBUGLINES
	DrawGizmo(aSpace.Row(3), aScale, aSpace);
#endif
}

void DebugDrawer::Render(Camera* aCamera)
{
#if USEDEBUGLINES
	if (myIsWorking)
	{
		ID3D11DeviceContext* context = myFramework->GetContext();

		D3D11_MAPPED_SUBRESOURCE bufferData;
		WIPE(bufferData);

		HRESULT result = context->Map(myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			SYSERROR("Oh no, buffer ded", "vertexbuffer");
			myIsWorking = false;
			return;
		}
		if (!myPoints.empty())
		{
			memcpy(bufferData.pData, &myPoints[0], sizeof(V4F) * myPointCount);
		}
		context->Unmap(myVertexBuffer, 0);

		result = context->Map(myConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			SYSERROR("Oh no, bluffer ded", "constantbuffer");
			return;
		}
		CameraBuffer* buff = reinterpret_cast<CameraBuffer*>(bufferData.pData);
		buff->myToCamera = CommonUtilities::Matrix4x4<float>::Transpose(CommonUtilities::Matrix4x4<float>::GetFastInverse(aCamera->GetTransform()));
		buff->myToProjection = CommonUtilities::Matrix4x4<float>::Transpose(aCamera->GetProjection(false));
		context->Unmap(myConstantBuffer, 0);


		context->IASetInputLayout(myInputLayout);
		context->PSSetShader(myPixelShader, nullptr, 0);
		context->VSSetShader(myVertexShader, nullptr, 0);
		context->GSSetShader(nullptr, nullptr, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);


		UINT stride = sizeof(V4F);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);

		context->VSSetConstantBuffers(0, 1, &myConstantBuffer);

		context->PSSetConstantBuffers(0, 1, &myColorBuffer);
		size_t at = 0;
		auto DrawTo = [&](size_t aNumber)
		{
			context->Draw(CAST(UINT, aNumber - at), CAST(UINT, at));
			at = aNumber;
		};
		auto SetColor = [&](V4F aColor)
		{
			if (SUCCEEDED(context->Map(myColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData)))
			{
				*((V4F*)bufferData.pData) = aColor;
				context->Unmap(myColorBuffer, 0);
			}
		};

		for (auto& i : myColorRanges)
		{
			SetColor(i.first);
			DrawTo(i.second);
		}
		SetColor(myColor);
		DrawTo(myPointCount);

		myPointCount = 0;
		myPoints.clear();
		myColorRanges.clear();
	}
#endif // !_RETAIL
}
