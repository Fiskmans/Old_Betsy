#include "pch.h"
#include "PlayAnimation.h"

PlayAnimation::PlayAnimation(Scene* aScene)
{
	myTarget.SetScene(aScene);
}

PlayAnimation::PlayAnimation(FiskJSON::Object& aObject, Scene* aScene) : PlayAnimation(aScene)
{
	myScene = aScene;
	aObject["AnimationIndex"].GetIf(myAnimation);
	myTarget.Load(aObject, aScene);
}

void PlayAnimation::Update(float aTime)
{
	if (myTarget.Get())
	{
		if (myIsplaying)
		{
			if (!IsActive(aTime))
			{
				myIsplaying = false;
			}
		}
		else
		{
			if (IsActive(aTime))
			{
				myIsplaying = true;
				if (myTarget.Get())
				{
					myTarget.Get()->SetAnimation(myAnimation);
				}
			}
		}
	}
}

void PlayAnimation::AppendTo(FiskJSON::Object& aObject)
{
	FiskJSON::Object* me = new FiskJSON::Object();
	AppendCommon(*me);
	myTarget.AppendTo(*me);
	me->AddValueChild("AnimationIndex", myAnimation);
	aObject.PushChild(me);
}

void PlayAnimation::Edit()
{
	myTarget.Edit();
#if USEIMGUI
	ImGui::InputInt("Animation to play", &myAnimation);
#endif // USEIMGUI
}

const char* PlayAnimation::GetType()
{
	return "Play Animation";
}
