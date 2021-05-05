#pragma once
#include "Vector.hpp"
#include "Observer.hpp"
#include "Matrix.hpp"

class Sprite;
class Scene;

class SpriteInstance
{
public:
	SpriteInstance(Sprite* aSprite);
	~SpriteInstance();

	Sprite* GetSprite();

	void SetPosition(const CommonUtilities::Vector2<float>& aPosition);
	void SetPosition(float aPositionX, float aPositionY);
	void SetScale(const CommonUtilities::Vector2<float>& aScale);
	void Rotate(const float aRotation);
	void SetRotation(const float aRotation);
	void SetPivot(const CommonUtilities::Vector2<float>& aPivot);
	void SetColor(const CommonUtilities::Vector4<float>& aColor);
	void SetUVMinMax(const CommonUtilities::Vector2<float>& aMin, const CommonUtilities::Vector2<float>& aMax);
	void SetUVMinMaxInTexels(const CommonUtilities::Vector2<float>& aMin, const CommonUtilities::Vector2<float>& aMax);
	void SetUVMinMax(const CommonUtilities::Vector4<float>& aMinMax);
	void SetDepth(float aDepth);
	void SetSize(V2F aSize);
	void SetSizeInPixel(V2F aSize);

	const M44F GetTransform() const;
	const M44F GetPivotTransform() const;

	const V2F GetPosition() const;
	const V4F& GetColor() const;
	const V2F GetScale() const;
	const V2F& GetPivot() const;
	const V4F& GetUVMinMax() const;
	const V2F GetImageSize() const;
	const V2F GetSizeWithScale() const;
	const V2F GetSizeOnScreen() const;
	const float GetDepth() const;

	void AddToScene();
	void RemoveFromScene();
	bool HasBeenAddedToScene();

#if USEIMGUI
	void ImGui();
#endif

private:

	Sprite* mySprite;

	V4F myColor;
	V2F myPivot;

	V4F myUVMinMax;

	V2F myPosition;

	bool myIsListening;
	bool myIsAddedToScene;

	float myRotation;
	float myDepth;

	V2F myScale;
	V2F myBaseScale;
};