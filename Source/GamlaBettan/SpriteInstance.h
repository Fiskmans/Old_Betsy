#pragma once
#include "Observer.hpp"

class Sprite;
class Scene;

class SpriteInstance
{
public:
	SpriteInstance(Sprite* aSprite);
	~SpriteInstance();

	Sprite* GetSprite();

	void SetPosition(const V2f& aPosition);
	void SetPosition(float aPositionX, float aPositionY);
	void SetScale(const V2f& aScale);
	void Rotate(const float aRotation);
	void SetRotation(const float aRotation);
	void SetPivot(const V2f& aPivot);
	void SetColor(const V4F& aColor);
	void SetUVMinMax(const V2f& aMin, const V2f& aMax);
	void SetUVMinMaxInTexels(const V2f& aMin, const V2f& aMax);
	void SetUVMinMax(const V4F& aMinMax);
	void SetDepth(float aDepth);
	void SetSize(V2f aSize);
	void SetSizeInPixel(V2f aSize);

	const M44f GetTransform() const;
	const M44f GetPivotTransform() const;

	const V2f GetPosition() const;
	const V4F& GetColor() const;
	const V2f GetScale() const;
	const V2f& GetPivot() const;
	const V4F& GetUVMinMax() const;
	const V2f GetImageSize() const;
	const V2f GetSizeWithScale() const;
	const V2f GetSizeOnScreen() const;
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
	V2f myPivot;

	V4F myUVMinMax;

	V2f myPosition;

	bool myIsListening;
	bool myIsAddedToScene;

	float myRotation;
	float myDepth;

	V2f myScale;
	V2f myBaseScale;
};