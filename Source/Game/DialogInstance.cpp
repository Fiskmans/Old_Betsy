#include "pch.h"
#include "SpriteFontInclude.h"
#include "DialogInstance.h"
#include "SpriteInstance.h"
#include "SpriteRenderer.h"
#include "PostMaster.hpp"
#include "Sprite.h"
#include "TimeHandler.h"


DialogInstance::DialogInstance() : 
	myDialog(nullptr),
	myCurrentIndex(0),
	myText(nullptr),
	myTitle(nullptr)
{
}

DialogInstance::~DialogInstance()
{
	myDialog = nullptr;
	UnsubscribeToMessages();
}

void DialogInstance::Init(DirectX::SpriteFont* aFont, const DialogData* someDialog, const V2F& aBuffer, SpriteInstance* aBackground)
{
	myDialog = someDialog;
	myBackgroundBuffer = aBuffer;
	myBackground = aBackground;
	myCurrentIndex = 0;
	mySpriteFontPtr = aFont;

	myTitle = &myDialog->data[myDialog->currentIndex][myCurrentIndex].speaker;
	myText = &myDialog->data[myDialog->currentIndex][myCurrentIndex].text;

	myScale = V2F(1.f, 1.f);
	myTitleScale= V2F(1.2f, 1.2f);

	SubscribeToMessages();

	TimeHandler::GetInstance().PauseTime();
}

void DialogInstance::Render(DirectX::SpriteBatch* aSpriteBatch)
{
	if (!mySpriteFontPtr)
	{
		SYSERROR("Oh no text no have font :c", "");
		return;
	}

	if (myShouldDraw && myText && myTitle)
	{
		if (myBackground)
		{
			myBackground->SetSizeInPixel((GetSize() + myBackgroundBuffer * 2.f));
			myBackground->SetPosition((myPosition / Sprite::ourWindowSize));
			myBackground->SetRotation(myRotation);

			ourBackgroundRendererPtr->Render({ myBackground });
		}

		mySpriteFontPtr->DrawString(aSpriteBatch, myText->c_str(), ToShitVector(myPosition - (myBackgroundBuffer * myPivot) + (myBackgroundBuffer * 0.5f) + V2F(0.f, GetTitleSize().y)), ToShitVector(myColor), myRotation, ToShitVector(GetSize() * myPivot), ToShitVector(myScale), CAST(DirectX::SpriteEffects, myEffect));

		if (!myTitle->empty())
		{
			mySpriteFontPtr->DrawString(aSpriteBatch, myTitle->c_str(), ToShitVector(myPosition - (myBackgroundBuffer * myPivot) + (myBackgroundBuffer * 0.5f) + GetTitleSize() * (myPivot * V2F(myTitleScale.x - myScale.x, 0.5f))), ToShitVector(myTitleColor), myRotation, ToShitVector(GetSize() * myPivot), ToShitVector(myTitleScale), CAST(DirectX::SpriteEffects, myEffect));

		}
	}
}

void DialogInstance::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::InputLeftMouseHit)
	{
		myCurrentIndex++;

		if (!ShouldBeRemoved())
		{
			myText = &myDialog->data[myDialog->currentIndex][myCurrentIndex].text;
			myTitle = &myDialog->data[myDialog->currentIndex][myCurrentIndex].speaker;
		}
		else
		{
			TimeHandler::GetInstance().ResumeTime();

			PostMaster::GetInstance()->SendMessages(MessageType::DialogueOver);
		}
	}

}

bool DialogInstance::ShouldBeRemoved() const
{
	return (myCurrentIndex >= myDialog->data[myDialog->currentIndex].size());
}

V2F DialogInstance::GetSize() const
{
	return TextInstance::GetSize(*myText, *myTitle);
}

const std::wstring& DialogInstance::GetWideTitle() const
{
	return *myTitle;
}

std::string DialogInstance::GetSlimTitle() const
{
	return std::string(myTitle->begin(), myTitle->end());
}

const std::wstring& DialogInstance::GetWideText() const
{
	return *myText;
}

std::string DialogInstance::GetSlimText() const
{
	return std::string(myText->begin(), myText->end());
}

void DialogInstance::SetText(const std::wstring& aText)
{
	SYSERROR("!!This function is not relevant so it won't do anything!!", "");
}

void DialogInstance::SetText(const std::string& aText)
{
	SYSERROR("!!This function is not relevant so it won't do anything!!", "");
}

void DialogInstance::SetTitle(const std::wstring& aTitle)
{
	SYSERROR("!!This function is not relevant so it won't do anything!!", "");
}

void DialogInstance::SetTitle(const std::string& aTitle)
{
	SYSERROR("!!This function is not relevant so it won't do anything!!", "");
}

V2F DialogInstance::GetTitleSize() const
{
	return TextInstance::GetSize(L"", *myTitle);
}

void DialogInstance::SubscribeToMessages()
{
	SubscribeToMessage(MessageType::InputLeftMouseHit);
}

void DialogInstance::UnsubscribeToMessages()
{
	UnSubscribeToMessage(MessageType::InputLeftMouseHit);
}
