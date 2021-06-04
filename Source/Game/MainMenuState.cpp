#include "pch.h"
#include "MainMenuState.h"
#include "GraphicEngine.h"
#include "DirectX11Framework.h"
#include "SpriteInstance.h"
#include <Xinput.h>
#include "AssetManager.h"


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
	myIntroHasPlayed = false;
	myGameTitleImage = nullptr;
	myMousePointer = nullptr;
	myShouldRemoveVideo = true;
}

MainMenuState::~MainMenuState()
{
	Deactivate();

	delete myMousePointer;
	delete myGameTitleImage;

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

	if (!myMousePointer)
	{
		myMousePointer = aSpritefactory->CreateSprite("ui/mouse.dds");
	}

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
	std::vector<SpriteInstance*> sprites;
	sprites.push_back(myGameTitleImage);

	sprites.push_back(myPlayButton.GetCurrentSprite());
	sprites.push_back(myExitButton.GetCurrentSprite());
	sprites.push_back(myCreditButton.GetCurrentSprite());
	//sprites.push_back(myOptionsButton.GetCurrentSprite());

	sprites.push_back(myMousePointer);

	aGraphicsEngine->RenderMovie(sprites);
}

void MainMenuState::Activate()
{
	myIsActive = true;
	myShouldRemoveVideo = true;

	PostMaster::GetInstance().SendMessages(MessageType::MainMenuStateActivated);
}

void MainMenuState::Deactivate()
{
	myIsActive = false;
}

void MainMenuState::Unload()
{
}

void MainMenuState::InitLayout(SpriteFactory* aSpritefactory)
{
	FiskJSON::Object& root = AssetManager::GetInstance().GetJSON("menu/MainMenuLayout.json").GetAsJSON();

	std::string imagesPath = root["ImagesPath"].Get<std::string>();

	myGameTitleImage = aSpritefactory->CreateSprite(imagesPath + "\\" + root["GameTitleImage"]["name"].Get<std::string>());
	myGameTitleImage->SetPosition({ root["GameTitleImage"]["PosX"].Get<float>(),  root["GameTitleImage"]["PosY"].Get<float>() });
	myGameTitleImage->SetUVMinMaxInTexels(V2f(0, 0), V2f(1920.f, 1080.f));
	myGameTitleImage->SetSize(V2f(1, 1));


	myPlayButton.Init(imagesPath, root["StartButton"]["name"].Get<std::string>(), { root["StartButton"]["PosX"].Get<float>(),  root["StartButton"]["PosY"].Get<float>() }, V2f(0.545f, 1.f), aSpritefactory);
	myExitButton.Init(imagesPath, root["ExitButton"]["name"].Get<std::string>(), { root["ExitButton"]["PosX"].Get<float>(),  root["ExitButton"]["PosY"].Get<float>() }, V2f(0.545f, 1.f), aSpritefactory);
	myCreditButton.Init(imagesPath, root["CreditsButton"]["name"].Get<std::string>(), { root["CreditsButton"]["PosX"].Get<float>(),  root["CreditsButton"]["PosY"].Get<float>() }, V2f(0.545f, 1.f), aSpritefactory);
	//myOptionsButton.Init(imagesPath, mainMenuDoc["OptionsButton"]["name"].GetString(), { mainMenuDoc["OptionsButton"]["PosX"].GetFloat(),  mainMenuDoc["OptionsButton"]["PosY"].GetFloat() }, V2f(0.545f, 1.f), aSpritefactory);


	if (!myGameStateToStart)
	{
		myGameStateToStart = CreateGameState(0);
	}

	myPlayButton.SetOnPressedFunction([this]() -> void
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

	myExitButton.SetOnPressedFunction([this]() -> void
		{
			bool mainState = true;
			PostMaster::GetInstance().SendMessages(MessageType::PopState,&mainState);
		});
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
