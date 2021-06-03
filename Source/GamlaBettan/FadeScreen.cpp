#include "pch.h"
#include "FadeScreen.h"
#include "SpriteInstance.h"


void FadeScreen::Update(float aTime)
{
	if (IsActive(aTime))
	{
		float progress = GetProgress(aTime);
		if (!myOut)
		{
			progress = 1.f - progress;
		}
		myFadeSprite->SetColor(V4F(1, 1, 1, progress));
	}
}

void FadeScreen::AppendTo(FiskJSON::Object& aObject)
{
	FiskJSON::Object* me = new FiskJSON::Object();
	AppendCommon(*me);
	me->AddValueChild("Fade",myOut);
	aObject.PushChild(me);
}

void FadeScreen::Edit()
{
#if USEIMGUI
	ImGui::Text("Fade: ");
	ImGui::SameLine();
	if (myOut)
	{
		if (ImGui::Button("out"))
		{
			myOut = false;
		}
	}
	else
	{
		if (ImGui::Button("in"))
		{
			myOut = true;
		}
	}
#endif // USEIMGUI
}

const char* FadeScreen::GetType()
{
	return "SceenFade";
}
