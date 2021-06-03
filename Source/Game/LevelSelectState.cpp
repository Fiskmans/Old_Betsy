#include <pch.h>
#include "LevelSelectState.h"
#include "GraphicEngine.h"
#include "DirectX11Framework.h"
#include "SpriteInstance.h"
#include "video.h"
#include <Xinput.h>
#include "GameState.h"


template<typename>
struct array_size;
template<typename T, size_t N>
struct array_size<std::array<T, N> > {
	static size_t const size = N;
};
#define SIZEOFARRAY(arg) array_size<decltype(arg)>::size


LevelSelectState::LevelSelectState(bool aShouldDeleteOnPop) :
	BaseState(aShouldDeleteOnPop),
	myStateInitData{ nullptr, nullptr, nullptr }
{
	SetUpdateThroughEnabled(false);
	SetDrawThroughEnabled(false);
	myVideoPtr = nullptr;
	myMousePointerPtr = nullptr;
}

LevelSelectState::~LevelSelectState()
{
	Deactivate();

	WIPE(*this);
}

void LevelSelectState::Update(const float aDeltaTime)
{
	if (myVideoPtr != nullptr)
	{
		(*myVideoPtr)->Update(myStateInitData.myFrameWork->GetContext(), aDeltaTime);
	}
}

void LevelSelectState::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::InputMouseMoved)
	{
		myMousePointerPtr->SetPosition(CommonUtilities::Vector2<float>(aMessage.myFloatValue, aMessage.myFloatValue2));
	}
}

bool LevelSelectState::Init(InputManager* aInputManager, ModelLoader* aModelLoader, SpriteFactory* aSpritefactory, LightLoader* aLightLoader, WindowHandler* aWindowHandler,
	DirectX11Framework* aFramework, AudioManager* aAudioManager, SpriteRenderer* aSpriteRenderer, Video** aVideoToKeepPlaying, SpriteInstance* aMousePointer)
{
	myIsMain = false;

	myMousePointerPtr = aMousePointer;

	myStateInitData.myFrameWork = aFramework;
	myStateInitData.myInputManager = aInputManager;
	myStateInitData.myLightLoader = aLightLoader;
	myStateInitData.myModelLoader = aModelLoader;
	myStateInitData.mySpriteFactory = aSpritefactory;
	myStateInitData.myWindowHandler = aWindowHandler;
	myStateInitData.myAudioManager = aAudioManager;
	myStateInitData.mySpriteRenderer = aSpriteRenderer;

	InitLayout(aSpritefactory);

	myVideoPtr = aVideoToKeepPlaying;

	return true;
}

void LevelSelectState::Render(CGraphicsEngine* aGraphicsEngine)
{
	std::vector<SpriteInstance*> sprites;

	if (myVideoPtr != nullptr)
	{
		sprites.push_back((*myVideoPtr)->GetSpriteInstance());
	}
	sprites.push_back(myBackButton.GetCurrentSprite());

	for (auto& button : myLevelButtons)
	{
		sprites.push_back(button.GetCurrentSprite());
	}
	sprites.push_back(myMousePointerPtr);

	aGraphicsEngine->RenderMovie(sprites);
}

void LevelSelectState::Activate()
{
	PostMaster::GetInstance()->Subscribe(MessageType::InputMouseMoved, this);
	myBackButton.Subscribe();
	for (auto& button : myLevelButtons)
	{
		button.Subscribe();
	}

	myIsActive = true;
}

void LevelSelectState::Deactivate()
{
	PostMaster::GetInstance()->UnSubscribe(MessageType::InputMouseMoved, this);

	myBackButton.Unsubscribe();

	for (auto& button : myLevelButtons)
	{
		button.Unsubscribe();
	}
	myIsActive = false;
}

void LevelSelectState::Unload()
{
}


GameState* LevelSelectState::CreateGameState(const int& aStartLevel)
{
	GameState* state = new GameState();

	if (!state->Init(myStateInitData.myWindowHandler, myStateInitData.myInputManager,
		myStateInitData.mySpriteFactory, myStateInitData.myLightLoader, myStateInitData.myFrameWork, myStateInitData.myAudioManager, myStateInitData.mySpriteRenderer))
	{
		delete state;
		return nullptr;
	}

	if (myVideoPtr != nullptr)
	{
		(*myVideoPtr)->Stop();
		(*myVideoPtr)->Destroy();
		delete *myVideoPtr;
		*myVideoPtr = nullptr;
		myVideoPtr = nullptr;
	}

	state->SetMain(true);
	state->LoadLevel(aStartLevel);

	return state;
}


void LevelSelectState::InitLayout(SpriteFactory* aSpritefactory)
{
	SYSERROR("Level select no longer works");
}
