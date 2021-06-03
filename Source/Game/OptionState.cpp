#include "pch.h"
#include "OptionState.h"
#include "GraphicEngine.h"
#include "DirectX11Framework.h"
#include "SpriteInstance.h"
#include "video.h"
#include <Xinput.h>


template<typename>
struct array_size;
template<typename T, size_t N>
struct array_size<std::array<T, N> > {
	static size_t const size = N;
};
#define SIZEOFARRAY(arg) array_size<decltype(arg)>::size


OptionState::OptionState(bool aShouldDeleteOnPop) :
	BaseState(aShouldDeleteOnPop),
	myStateInitData{ nullptr, nullptr, nullptr }
{
	SetUpdateThroughEnabled(false);
	SetDrawThroughEnabled(false);
	myVideoPtr = nullptr;
	myMousePointerPtr = nullptr;
}

OptionState::~OptionState()
{
	Deactivate();

	WIPE(*this);
}

void OptionState::Update(const float aDeltaTime)
{
	if (myVideoPtr != nullptr)
	{
		myVideoPtr->Update(myStateInitData.myFrameWork->GetContext(), aDeltaTime);
	}
}

void OptionState::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::InputMouseMoved)
	{
		myMousePointerPtr->SetPosition(CommonUtilities::Vector2<float>(aMessage.myFloatValue, aMessage.myFloatValue2));
	}
}

bool OptionState::Init(SpriteFactory* aSpritefactory, DirectX11Framework* aFramework, SpriteRenderer* aSpriteRenderer, Video* aVideoToKeepPlaying, SpriteInstance* aMousePointer)
{
	myIsMain = false;

	myMousePointerPtr = aMousePointer;

	myStateInitData.myFrameWork = aFramework;
	myStateInitData.mySpriteFactory = aSpritefactory;
	myStateInitData.mySpriteRenderer = aSpriteRenderer;

	InitLayout(aSpritefactory);

	myVideoPtr = aVideoToKeepPlaying;

	return true;
}

void OptionState::Render(CGraphicsEngine* aGraphicsEngine)
{
	std::vector<SpriteInstance*> sprites;

	if (myVideoPtr != nullptr)
	{
		sprites.push_back(myVideoPtr->GetSpriteInstance());
	}
	sprites.push_back(myBackButton.GetCurrentSprite());

	for (auto& button : myScreensizeButtons)
	{
		sprites.push_back(button.GetCurrentSprite());
	}
	sprites.push_back(myMousePointerPtr);

	aGraphicsEngine->RenderMovie(sprites);
}

void OptionState::Activate()
{
	PostMaster::GetInstance()->Subscribe(MessageType::InputMouseMoved, this);
	myBackButton.Subscribe();
	for (auto& button : myScreensizeButtons)
	{
		button.Subscribe();
	}

	myIsActive = true;
}

void OptionState::Deactivate()
{
	PostMaster::GetInstance()->UnSubscribe(MessageType::InputMouseMoved, this);

	myBackButton.Unsubscribe();

	for (auto& button : myScreensizeButtons)
	{
		button.Unsubscribe();
	}
	myIsActive = false;
}

void OptionState::Unload()
{
}

void OptionState::InitLayout(SpriteFactory* aSpritefactory)
{
	SYSERROR("Options no longer works");
}
