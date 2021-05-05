#pragma once
#include "BaseState.h"
#include "Button.h"
#include "Observer.hpp"
#include <array>
#include "Publisher.hpp"

class DirectX11Framework;
class AudioManager;
class Video;
class SpriteRenderer;

class LevelSelectState : public BaseState, public Publisher, public Observer
{
public:
	LevelSelectState(bool aShouldDeleteOnPop = true);
	~LevelSelectState();
	virtual void Update(const float aDeltaTime) override;
	virtual void RecieveMessage(const Message& aMessage) override;
	bool Init(InputManager* aInputManager, ModelLoader* aModelLoader, SpriteFactory* aSpritefactory, LightLoader* aLightLoader, WindowHandler* aWindowHandler,
		DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer, Video** aVideoToKeepPlaying, SpriteInstance* aMousePointer);
	virtual void Render(CGraphicsEngine* aGraphicsEngine) override;

	virtual void Activate()override;
	virtual void Deactivate()override;

	virtual void Unload()override;

	class GameState* CreateGameState(const int& aStartLevel);

private:
	void InitLayout(SpriteFactory* aSpritefactory);

	struct StateInitData
	{
		InputManager* myInputManager;
		WindowHandler* myWindowHandler;
		ModelLoader* myModelLoader;
		LightLoader* myLightLoader;
		SpriteFactory* mySpriteFactory;
		DirectX11Framework* myFrameWork;
		AudioManager* myAudioManager;
		SpriteRenderer* mySpriteRenderer;
	} myStateInitData;

	Button myBackButton;

	std::array<Button, 8> myLevelButtons;

	int selectionIndex = 0;

	SpriteInstance* myMousePointerPtr;

	Video** myVideoPtr;

	bool myIsActive = false;
};