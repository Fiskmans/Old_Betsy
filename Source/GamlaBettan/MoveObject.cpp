#include "pch.h"
#include "MoveObject.h"
#include "ModelInstance.h"
#include "CommonUtilities\Ray.hpp"
#include "SlabRay.h"
#include "Scene.h"
#include "Model.h"

MoveObject::MoveObject(V3F aFrom, V3F aTo, Scene* aScene) : myPath(aTo)
{
	myTarget.SetScene(aScene);
	myScene = aScene;
	myPath.SetStart(aFrom);
	myPath.AllowEditStart();
}

MoveObject::MoveObject(CinematicPath aPath)
{
	myPath = aPath;
	myPath.AllowEditStart();
}

MoveObject::MoveObject(FiskJSON::Object& aObject, Scene* aScene)
{
	myPath.ParseFrom(aObject["Path"]);
	myTarget.Load(aObject,aScene);
}

void MoveObject::SetModel(ModelInstance* aModel)
{
	myTarget.Set(aModel);
}

void MoveObject::Update(float aTime)
{
	if (IsActive(aTime))
	{
		float progress = GetProgress(aTime);
		if (myTarget.Get())
		{
			myTarget.Get()->SetPosition(V4F(myPath.Evaluate(progress),1));
		}
	}
}

void MoveObject::AppendTo(FiskJSON::Object& aObject)
{
	FiskJSON::Object* me = new FiskJSON::Object();
	AppendCommon(*me);
	FiskJSON::Object* path = new FiskJSON::Object();
	myPath.AppendTo(path);
	me->AddChild("Path", path);
	myTarget.AppendTo(*me);
	aObject.PushChild(me);
	me->AddValueChild("CustomName", std::string(myName));
}

void MoveObject::Edit()
{
#if USEIMGUI
	if (myTarget.Get())
	{
		V3F at = myTarget.Get()->GetPosition();
		myPath.Edit(&at);
	}
	else
	{
		myPath.Edit();
	}
	myTarget.Edit();
	if (ImGui::Button("Reset Model"))
	{
		if (myTarget.Get())
		{
			myTarget.Get()->SetPosition(myTarget.GetPosition());
		}
	}
#endif // USEIMGUI
}

const char* MoveObject::GetType()
{
	return "MoveObject";
}
