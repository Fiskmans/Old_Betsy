#include "pch.h"
#include "SpriteInstance.h"
#include "Sprite.h"
#include "WindowHandler.h"


SpriteInstance::SpriteInstance(Sprite* aSprite)
{
	mySprite = aSprite;

	myPivot = V2f(0.0f, 0.0f);

	myColor = V4F(1.0f, 1.0f, 1.0f, 1.0f);

	myUVMinMax = V4F(0.f, 0.f, 1.f, 1.f);

	SetScale({ 1.f, 1.f });

	myIsListening = false;

	myRotation = 0;

	myScale = { 1.f, 1.f };

	V2ui windowSize = WindowHandler::GetInstance().GetSize();
	myBaseScale = V2f(
		aSprite->GetSpriteData().mySize.x / static_cast<float>(windowSize.x),
		aSprite->GetSpriteData().mySize.y / static_cast<float>(windowSize.y)) * 2.f;

	myDepth = 0.f;
}

SpriteInstance::~SpriteInstance()
{

}

Sprite* SpriteInstance::GetSprite()
{
	return mySprite;
}

void SpriteInstance::SetPosition(const V2f& aPosition)
{
	myPosition = aPosition;
}

void SpriteInstance::SetPosition(float aPositionX, float aPositionY)
{
	SetPosition(V2f(aPositionX, aPositionY));
}

void SpriteInstance::SetScale(const V2f& aScale)
{
	myScale = aScale;
}

void SpriteInstance::Rotate(const float aRotation)
{
	myRotation += aRotation;
}

void SpriteInstance::SetRotation(const float aRotation)
{
	myRotation = aRotation;
}

void SpriteInstance::SetPivot(const V2f& aPivot)
{
	myPivot = aPivot;
}

void SpriteInstance::SetColor(const CommonUtilities::Vector4<float>& aColor)
{
	myColor = aColor;
}

void SpriteInstance::SetUVMinMax(const V2f& aMin, const V2f& aMax)
{
	myUVMinMax.x = aMin.x;
	myUVMinMax.y = aMin.y;

	myUVMinMax.z = aMax.x;
	myUVMinMax.w = aMax.y;
}

void SpriteInstance::SetUVMinMaxInTexels(const V2f& aMin, const V2f& aMax)
{
	V2ui windowSize = WindowHandler::GetInstance().GetSize();

	myUVMinMax.x = aMin.x / (myBaseScale.x * static_cast<float>(windowSize.x)) * 2;
	myUVMinMax.y = aMin.y / (myBaseScale.y * static_cast<float>(windowSize.y)) * 2;

	myUVMinMax.z = aMax.x / (myBaseScale.x * static_cast<float>(windowSize.x)) * 2;
	myUVMinMax.w = aMax.y / (myBaseScale.y * static_cast<float>(windowSize.y)) * 2;
}

void SpriteInstance::SetUVMinMax(const CommonUtilities::Vector4<float>& aMinMax)
{
	SetUVMinMax(V2f(aMinMax.x, aMinMax.y), V2f(aMinMax.z, aMinMax.w));
}

void SpriteInstance::SetDepth(float aDepth)
{
	myDepth = aDepth;
}

void SpriteInstance::SetSize(V2f aSize)
{
	myScale = aSize / myBaseScale * 2.f;
}

void SpriteInstance::SetSizeInPixel(V2f aSize)
{
	V2ui windowSize = WindowHandler::GetInstance().GetSize();
	myScale = (aSize / static_cast<V2f>(windowSize)) / myBaseScale * 2.f;
}

const CommonUtilities::Matrix4x4<float> SpriteInstance::GetTransform() const
{
	return M44f::CreateRotationAroundZ(myRotation) *
		M44f({ myScale.x * myBaseScale.x,	0.f,							0.f, 0.f,
				0.f,						myScale.y * myBaseScale.y,		0.f, 0.f,
				0.f,						0.f,							1.f, 0.f,
				(myPosition.x * 2.f - 1.f), -(myPosition.y * 2.f - 1.f),	0.f, 1.f });
}

const M44f SpriteInstance::GetPivotTransform() const
{
	return { 1.f,		 0.f,		0.f, 0.f,
			 0.f,		 1.f,		0.f, 0.f,
			 0.f,		 0.f,		1.f, 0.f,
			 -myPivot.x, myPivot.y, 0.f, 1.f };
}

const V2f SpriteInstance::GetPosition() const
{
	return myPosition;
}

const CommonUtilities::Vector4<float>& SpriteInstance::GetColor() const
{
	return myColor;
}

const V2f SpriteInstance::GetScale() const
{
	return myScale;
}

const V2f& SpriteInstance::GetPivot() const
{
	return myPivot;
}

const CommonUtilities::Vector4<float>& SpriteInstance::GetUVMinMax() const
{
	return myUVMinMax;
}

const V2f SpriteInstance::GetImageSize() const
{
	return { CAST(float, mySprite->GetSpriteData().mySize.x), CAST(float, mySprite->GetSpriteData().mySize.y) };
}

const V2f SpriteInstance::GetSizeWithScale() const
{
	return { CAST(float, mySprite->GetSpriteData().mySize.x) * GetScale().x, CAST(float, mySprite->GetSpriteData().mySize.y) * GetScale().y };
}

const V2f SpriteInstance::GetSizeOnScreen() const
{
	return GetSizeWithScale() / static_cast<V2f>(WindowHandler::GetInstance().GetSize());
}

const float SpriteInstance::GetDepth() const
{
	return myDepth;
}


void SpriteInstance::AddToScene()
{
	myIsAddedToScene = true;
}

void SpriteInstance::RemoveFromScene()
{
	myIsAddedToScene = false;
}

bool SpriteInstance::HasBeenAddedToScene()
{
	return myIsAddedToScene;
}

#if USEIMGUI
void SpriteInstance::ImGui()
{
	ImGui::Text("Sprite");


	V2ui windowSize = WindowHandler::GetInstance().GetSize();
	const float imScale = 0.4f;
	ImVec2 imageSize = ImVec2(imScale * myBaseScale.x * windowSize.x, imScale * myBaseScale.y * windowSize.y);
	float windWidth = ImGui::GetWindowWidth();
	if (imageSize.x > windWidth)
	{
		imageSize = ImVec2(windWidth, imageSize.y * (windWidth / imageSize.x));
	}
	ImVec2 cursorposlocal = ImGui::GetCursorPos();
	ImVec2 cursorpos = ImGui::GetCursorScreenPos();
	ImVec2 iopos = ImVec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
	Tools::ZoomableImGuiImage(mySprite->GetSpriteData().myTexture.GetAsTexture(), imageSize);
	V2f mpInImage = V2f((iopos.x - cursorpos.x) / imageSize.x, (iopos.y - cursorpos.y) / imageSize.y);

	bool mouseInside = mpInImage.x > 0.f && mpInImage.x < 1.f && mpInImage.y > 0.f && mpInImage.y < 1.f;

	if (mouseInside)
	{
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsKeyDown(VK_SHIFT))
		{
			SetPivot(mpInImage);
		}
	}
	ImGui::Text(PFSTRING, mySprite->GetSpriteData().myFilePath.c_str());
	ImGui::Text("width:" PFUINT " height:" PFUINT, mySprite->GetSpriteData().mySize.x, mySprite->GetSpriteData().mySize.y);
	ImGui::DragFloat2("Pivot", &myPivot.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat2("Position", &myPosition.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat2("Scale", &myScale.x, 0.01f, 0.0f, 1.0f);
	ImGui::Text("Resulting scale x:" PFFLOAT " y:" PFFLOAT, myScale.x*myBaseScale.x, myScale.y * myBaseScale.y);
	ImGui::DragFloat2("UVMin", &myUVMinMax.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat2("UVMax", &myUVMinMax.z, 0.01f, 0.0f, 1.0f);

	
	ImGui::DragFloat("Depth", &myDepth, 0.01f, 0.0f, 1.0f);
	ImGui::GetWindowDrawList()->AddRect(ImVec2(cursorpos.x + myUVMinMax.x * imageSize.x, cursorpos.y + myUVMinMax.y * imageSize.x), ImVec2(cursorpos.x + myUVMinMax.z * imageSize.x, cursorpos.y + myUVMinMax.w * imageSize.x), IM_COL32(255, 0, 0, 255));
	ImVec2 pivotPos = ImVec2(cursorpos.x + myPivot.x * imageSize.x, cursorpos.y + myPivot.y * imageSize.x);
	ImGui::GetWindowDrawList()->AddLine(ImVec2(pivotPos.x-20.f,pivotPos.y-20.f), ImVec2(pivotPos.x + 20.f, pivotPos.y + 20.f), IM_COL32(255, 0, 0, 255));
	ImGui::GetWindowDrawList()->AddLine(ImVec2(pivotPos.x - 20.f, pivotPos.y + 20.f), ImVec2(pivotPos.x - 20.f, pivotPos.y + 20.f), IM_COL32(255, 0, 0, 255));
}
#endif
