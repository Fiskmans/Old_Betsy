#pragma once
#include "GameWorld.h"
#include "StateStack.h"

class CGraphicsEngine;
class WindowHandler;
class ModelLoader;
class LightLoader;
struct HWND__;
typedef HWND__* HWND;
class InputManager;
class SpriteFactory;
struct ID3D11DeviceContext;

class Game
{
public:

	Game();
	~Game();

	bool Init(WindowHandler* aWindowHandler,CommonUtilities::InputHandler* aInputHandler, ModelLoader* aModelLoader, LightLoader* aLightLoader, SpriteFactory* aSpriteFactory,
		DirectX11Framework* aFramework, AudioManager* aAudioManager, class SpriteRenderer* aSpriteRenderer);
	bool Run(CGraphicsEngine* aEngine, float aDeltaTime);

	
private:
	

	float myTooFastLimit = 0.f;

	InputManager* myInputManager;

	StateStack myStateStack;

};

