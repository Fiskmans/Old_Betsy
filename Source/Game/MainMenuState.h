#pragma once
#include "GameState.h"
#include "BaseState.h"
#include "Button.h"

struct ID3D11DeviceContext;
class SpriteInstance;
class Video;
class OptionState;

class MainMenuState : public BaseState, public Observer
{
public:
	MainMenuState(bool aShouldDeleteOnPop = true);
	~MainMenuState();
	virtual void Update(const float aDeltaTime) override;
	virtual void RecieveMessage(const Message& aMessage) override;
	bool Init(InputManager* aInputManager, SpriteFactory* aSpritefactory, 
		LightLoader* aLightLoader, DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer);
	virtual void Render(CGraphicsEngine* aGraphicsEngine) override;

	virtual void Activate() override;
	virtual void Deactivate() override;

	virtual void Unload() override;

private:

	void InitLayout(SpriteFactory* aSpritefactory);

	GameState* CreateGameState(const int& aStartLevel);

	GameState* myGameStateToStart = nullptr;
	struct StateInitData
	{
		InputManager* myInputManager;
		LightLoader* myLightLoader;
		SpriteFactory* mySpriteFactory;
		DirectX11Framework* myFrameWork;
		AudioManager* myAudioManager;
		SpriteRenderer* mySpriteRenderer;
	} myStateInitData;

	std::vector<Button*> myButtons;

	SpriteInstance* myBackground;
	SpriteInstance* myMousePointer;
};

