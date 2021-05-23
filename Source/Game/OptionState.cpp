#include "pch.h"
#include "OptionState.h"
#include "GraphicEngine.h"
#include "DirectX11Framework.h"
#include "SpriteInstance.h"
#include "video.h"
#include <Xinput.h>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>


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
	rapidjson::Document optionsDoc;

#pragma warning(suppress : 4996)
	FILE* fp = fopen("Data\\Textures\\Gui\\OptionsMenulayout.json", "rb");
	char readBuffer[4096];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	optionsDoc.ParseStream(is);
	fclose(fp);

	std::string imagesPath = optionsDoc["ImagesPath"].GetString();

	myBackButton.Init(imagesPath, optionsDoc["BackButton"]["name"].GetString(), { optionsDoc["BackButton"]["PosX"].GetFloat(),  optionsDoc["BackButton"]["PosY"].GetFloat() }, V2F(0.545f, 1.f), aSpritefactory);

	myBackButton.SetOnPressedFunction([this]
	{
		Message message;
		message.myMessageType = MessageType::PopState;
		message.myBool = false;
		Publisher::SendMessages(message);
	});

	std::array<CommonUtilities::Vector2<unsigned int>, SIZEOFARRAY(myScreensizeButtons)> sizes =
	{
		CommonUtilities::Vector2<unsigned int>(1280U,720U),
		CommonUtilities::Vector2<unsigned int>(1366U,768U),
		CommonUtilities::Vector2<unsigned int>(1600U,900U),
		CommonUtilities::Vector2<unsigned int>(1920U,1080U)
	};


	imagesPath += "\\Resizes";

	for (rapidjson::SizeType i = 0; i < myScreensizeButtons.size(); i++)
	{
		myScreensizeButtons[i].Init(imagesPath,
 optionsDoc["ResolutionButtons"][i]["Name"].GetString(),
			{ optionsDoc["ResolutionButtons"][i]["PosX"].GetFloat(),  optionsDoc["ResolutionButtons"][i]["PosY"].GetFloat() }, V2F(0.455f + (0.081f * CAST(float, i)), 0.512f + (0.07f * CAST(float, i))), aSpritefactory);

		myScreensizeButtons[i].SetOnPressedFunction([sizes, i, this]
		{
			Message message;
			message.myMessageType = MessageType::ResizeWindow;
			message.myIntValue = sizes[i].x;
			message.myIntValue2 = sizes[i].y;
			Publisher::SendMessages(message);
		});
	}
}
