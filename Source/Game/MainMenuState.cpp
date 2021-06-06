#include "pch.h"
#include "MainMenuState.h"
#include "GraphicEngine.h"
#include "DirectX11Framework.h"
#include "SpriteInstance.h"
#include <Xinput.h>
#include "AssetManager.h"
#include "GamlaBettan\Scene.h"

MainMenuState::MainMenuState(bool aShouldDeleteOnPop) :
	BaseState(aShouldDeleteOnPop),
	Observer(
		{
			MessageType::InputMouseMoved
		}),
	myStateInitData{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }
{
	SetUpdateThroughEnabled(false);
	SetDrawThroughEnabled(false);
	myBackground = nullptr;
	myMousePointer = nullptr;
}

MainMenuState::~MainMenuState()
{
	Deactivate();
	for (Button* button : myButtons)
	{
		delete button;
	}
	myButtons.clear();
	delete myMousePointer;
	delete myBackground;

	WIPE(*this);
}

void MainMenuState::Update(const float aDeltaTime)
{
	if (myGameStateToStart)
	{
		myGameStateToStart->PreSetup(aDeltaTime);
	}

#if DIRECTTOGAME
	static bool first = true;

	if (first)
	{
		myPlayButton.TriggerOnPressed();
		first = false;
	}
#endif
}

void MainMenuState::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::InputMouseMoved)
	{
		myMousePointer->SetPosition(*reinterpret_cast<const V2f*>(aMessage.myData));
	}
}

bool MainMenuState::Init(InputManager* aInputManager, SpriteFactory* aSpritefactory,
	LightLoader* aLightLoader, DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer)
{
	myIsMain = true;

	myMousePointer = aSpritefactory->CreateSprite("ui/mouse.dds");

	myStateInitData.myFrameWork = aFramework;
	myStateInitData.myInputManager = aInputManager;
	myStateInitData.myLightLoader = aLightLoader;
	myStateInitData.mySpriteFactory = aSpritefactory;
	myStateInitData.myAudioManager = aAudioManager;
	myStateInitData.mySpriteRenderer = aSpriteRenderer;

	InitLayout(aSpritefactory);

	return true;
}

void MainMenuState::Render(CGraphicsEngine* aGraphicsEngine)
{
	aGraphicsEngine->RenderFrame();
}

void MainMenuState::Activate()
{
	for (Button* button : myButtons)
	{
		button->Enable();
	}

	Scene::GetInstance().AddToScene(myMousePointer);
	Scene::GetInstance().AddToScene(myBackground);
}

void MainMenuState::Deactivate()
{
	for (Button* button : myButtons)
	{
		button->Disable();
	}

	Scene::GetInstance().RemoveFromScene(myMousePointer);
	Scene::GetInstance().RemoveFromScene(myBackground);
}

void MainMenuState::Unload()
{
}

void MainMenuState::InitLayout(SpriteFactory* aSpritefactory)
{
	FiskJSON::Object& root = AssetManager::GetInstance().GetJSON("menu/MainMenuLayout.json").GetAsJSON();

	std::string imagesPath = root["ImagesPath"].Get<std::string>();

	myBackground = aSpritefactory->CreateSprite(imagesPath + "\\" + root["GameTitleImage"]["name"].Get<std::string>());
	myBackground->SetPosition({ root["GameTitleImage"]["PosX"].Get<float>(),  root["GameTitleImage"]["PosY"].Get<float>() });
	myBackground->SetUVMinMaxInTexels(V2f(0, 0), V2f(1920.f, 1080.f));
	myBackground->SetDepth(1.f);
	myBackground->SetSize(V2f(1, 1));

	Button* playButton = new Button();
	Button* exitButton = new Button();

	playButton->Init(imagesPath, root["StartButton"]["name"].Get<std::string>(), { root["StartButton"]["PosX"].Get<float>(),  root["StartButton"]["PosY"].Get<float>() }, V2f(0.545f, 1.f), aSpritefactory);
	exitButton->Init(imagesPath, root["ExitButton"]["name"].Get<std::string>(), { root["ExitButton"]["PosX"].Get<float>(),  root["ExitButton"]["PosY"].Get<float>() }, V2f(0.545f, 1.f), aSpritefactory);
	

	if (!myGameStateToStart)
	{
		myGameStateToStart = CreateGameState(0);
	}

	playButton->SetOnPressedFunction([this]() -> void
		{
			Message message;
			message.myMessageType = MessageType::PushState;
			if (myGameStateToStart)
			{
				message.myData = myGameStateToStart;
				myGameStateToStart = nullptr;
			}
			else
			{
				message.myData = CreateGameState(0);
			}

			PostMaster::GetInstance().SendMessages(message);
		});

	exitButton->SetOnPressedFunction([this]() -> void
		{
			bool mainState = true;
			PostMaster::GetInstance().SendMessages(MessageType::PopState,&mainState);
		});

	myButtons.push_back(playButton);
	myButtons.push_back(exitButton);
}

GameState* MainMenuState::CreateGameState(const int& aStartLevel)
{
	GameState* state = new GameState();

	if (state->Init(myStateInitData.myInputManager,
		myStateInitData.mySpriteFactory, myStateInitData.myLightLoader, myStateInitData.myFrameWork, myStateInitData.myAudioManager, myStateInitData.mySpriteRenderer) == false)
	{
		delete state;
		return nullptr;
	}
	state->SetMain(true);
	state->LoadLevel(aStartLevel);

	return state;
}
