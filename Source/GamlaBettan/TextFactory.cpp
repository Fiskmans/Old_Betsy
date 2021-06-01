#include "pch.h"
#include "TextFactory.h"
#include "TextInstance.h"
#include "SpriteFactory.h"
#include "AssetManager.h"

TextFactory::TextFactory() 
{
	myDevicePtr = nullptr;
	mySpriteFactoryPtr = nullptr;
}

TextFactory::~TextFactory()
{
	myDevicePtr = nullptr;
	mySpriteFactoryPtr = nullptr;
}

bool TextFactory::Init(SpriteRenderer* aRenderer, SpriteFactory* aSpriteFactory)
{
	if (!aRenderer)
	{
		SYSERROR("Text factory was not given a sprite renderer!");
		return false;
	}

	TextInstance::ourBackgroundRendererPtr = aRenderer;

	mySpriteFactoryPtr = aSpriteFactory;


	return true;
}

TextInstance* TextFactory::CreateText()
{
	return AssetManager::GetInstance().GetFont("default.spritefont").InstansiateText();
}

TextInstance* TextFactory::CreateToolTip(SpriteInstance* aBackground, const V2F& aBuffer, const std::string& someText, const std::string& aFontPath)
{
	TextInstance* toolTip = AssetManager::GetInstance().GetFont(aFontPath).InstansiateText();

	if (toolTip)
	{
		toolTip->SetText(someText);
		toolTip->SetBackground(aBackground);
		toolTip->SetBuffer(aBuffer);
	}

	return toolTip;
}

TextInstance* TextFactory::CreateToolTip(const std::string& aBackgroundPath, const V2F& aBuffer, const std::string& someText, const std::string& aFontPath)
{
	return CreateToolTip(mySpriteFactoryPtr->CreateSprite(aBackgroundPath), aBuffer, someText, aFontPath);
}
