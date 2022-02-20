#pragma once
#include "Observer.hpp"

class SpriteFactory;
class SpriteInstance;
class Scene;

class Button : public Observer
{
public:
	Button();
	~Button();
	void SetOnPressedFunction(const std::function<void(void)> aOnPressed);
	SpriteInstance* GetCurrentSprite();

	bool Init(const std::string& aFolderPath, const std::string& aButtonName, const CommonUtilities::Vector2<float>& aPosition,
		const V2f& aHitBoxSize, SpriteFactory* aSpriteFactory);

	bool IsHovered();
	bool IsPressed();

	void Disable();
	void Enable();

	void RecieveMessage(const Message& aMessage) override;

private:

	bool CheckHover(const V2f& aPosition);

	void TriggerOnPressed();

	void SetHovered();
	void SetPressed();
	void SetToNormal();

	void SetupBounds();
	enum ButtonState
	{
		Normal,
		Hover,
		Pressed
	};

	bool myIsEnabled;

	std::function<void(void)> myOnPressed;
	ButtonState myState;
	std::array<SpriteInstance*, 4> mySprites;

	CommonUtilities::Vector4<float> myBounds;
	CommonUtilities::Vector2<float> myPosition;
	CommonUtilities::Vector2<float> myScreenSize;
	CommonUtilities::Vector2<float> mySize;
};

