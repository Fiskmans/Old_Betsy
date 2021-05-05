#include "pch.h"
#include "ProgressBar.h"
#include "SpriteFactory.h"
#include "SpriteInstance.h"

SpriteFactory* ProgressBar::ourSpriteFactory = nullptr;

void ProgressBar::Init(const V2F& aPos, const int* aValueToRepresent, const int* aMaxVauleToRepresent, const std::string& aBarPath, bool aIsHorizontal, const std::string& aBackgroundPath, const std::string& aForegroundPath, bool aGoFromUVMax)
{
	myVaulePtr = aValueToRepresent;
	myMaxVaulePtr = aMaxVauleToRepresent;
	myIsHorizontal = aIsHorizontal;
	myFromUVMax = aGoFromUVMax;

	myBar = ourSpriteFactory->CreateSprite(aBarPath);
	myBar->SetPosition(aPos);
	myPos = aPos;

	if (!aForegroundPath.empty())
	{
		myForeground = ourSpriteFactory->CreateSprite(aForegroundPath);
		myForeground->SetPosition(aPos - (myForeground->GetSizeOnScreen() - myBar->GetSizeOnScreen()) * 0.5f);
	}
	if (!aBackgroundPath.empty())
	{
		myBackground = ourSpriteFactory->CreateSprite(aBackgroundPath);
		myBackground->SetPosition(aPos - (myBackground->GetSizeOnScreen() - myBar->GetSizeOnScreen()) * 0.5f);
	}
}

void ProgressBar::Update()
{
	V4F axis;
	if (myIsHorizontal)
	{
		axis = V4F(1, 0, 1, 0);
	}
	else
	{
		axis = V4F(0, 1, 0, 1);
	}

	V4F dir;
	if (myFromUVMax)
	{
		dir = V4F(0, 0, 1, 1);
	}
	else
	{
		dir = V4F(-1, -1, 0, 0);
	}

	const float percentage = CLAMP(0.f, 1.f, (CAST(float, (*myMaxVaulePtr)) - CAST(float, (*myVaulePtr))) / CAST(float, (*myMaxVaulePtr)));

	V4F a = (axis * dir * percentage);

	const V4F uv = V4F(0, 0, 1, 1) - a;

	myBar->SetUVMinMax(uv);
	myBar->SetScale(V2F(1.f, 1.f) - V2F(axis) * percentage);
}

SpriteInstance* ProgressBar::GetBackground()
{
	return myBackground;
}

SpriteInstance* ProgressBar::GetBar()
{
	return myBar;
}

SpriteInstance* ProgressBar::GetForeground()
{
	return myForeground;
}

void ProgressBar::SetSpriteFactory(SpriteFactory* aFactory)
{
	ourSpriteFactory = aFactory;
}

void ProgressBar::SetPivot(const V2F& aPivot)
{
	myBar->SetPivot(aPivot);
	//Do position stuff here

	if (myForeground)
	{
		myForeground->SetPivot(aPivot);
	}
	if(myBackground)
	{
		myBackground->SetPivot(aPivot);
	}
}
