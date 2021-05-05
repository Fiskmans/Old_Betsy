#pragma once
#include <functional>
#include "Vector4.hpp"
#include "Vector2.hpp"
#include "Observer.hpp"
#include <array>

class SpriteFactory;
class SpriteInstance;
class Scene;

class Button : public Observer
{
public:
	Button();
	~Button();
	void SetOnPressedFunction(const std::function<void(void)> aOnPressed);
	void TriggerOnPressed();
	void SetActive();
	void SetPressed();
	void SetToNormal();
	void ActuallyEnable();
	void ActuallyDisable();
	SpriteInstance* GetCurrentSprite();
	SpriteInstance* GetTopBorderSprite();
	SpriteInstance* GetLowerBorderSprite();

	bool CheckHover(float aMouseX, float aMouseY);
	bool Init(const std::string& aNormalPath, const std::string& aHoveredPath, const std::string& aPressedPath, const std::string& aDisabledPath, const CommonUtilities::Vector2<float>& aPosition,
		const V2F& aHitBoxSize = V2F(1.f, 1.f), SpriteFactory* aSpriteFactory = ourSpriteFactoryPtr);
	bool Init(const std::string& aFolderPath, const std::string& aButtonName, const CommonUtilities::Vector2<float>& aPosition, 
		const V2F& aHitBoxSize = V2F(1.f, 1.f), SpriteFactory* aSpriteFactory = ourSpriteFactoryPtr);
	bool IsActive();
	bool IsPressed();
	void RecieveMessage(const Message& aMessage) override;

	void Subscribe();
	void Unsubscribe();

	static void SetSpriteFactory(SpriteFactory* aSpriteFactory);
	void SetScenePtr(Scene* aScenePtr);
	void RemoveAllSprites();
	//sorry...

	static SpriteFactory* GetSpriteFactory();
private:
	void SetupBounds();
	enum ButtonState
	{
		Normal,
		Hover,
		Pressed,
		Disabled
	};
	std::function<void(void)> myOnPressed;
	ButtonState myState;
	std::array<SpriteInstance*, 4> mySprites;

	CommonUtilities::Vector4<float> myBounds;
	CommonUtilities::Vector2<float> myPosition;
	CommonUtilities::Vector2<float> myScreenSize;
	CommonUtilities::Vector2<float> mySize;

	static SpriteFactory* ourSpriteFactoryPtr;
	bool myIsListening;
	bool myIsListeningToClick = false;
	Scene* myScenePtr;
};

