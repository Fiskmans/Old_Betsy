#include "pch.h"
#include "CameraPan.h"
#include "Camera.h"
#include "FiskJSON.h"
#include "DebugDrawer.h"
#if USEIMGUI
#include <imgui.h>
#endif

CameraPan::CameraPan(V3F aStart, V3F aEnd) : myPath(aEnd)
{
	myPath.SetStart(aStart);
}

CameraPan::CameraPan(CinematicPath aPath) : myPath(aPath)
{
	
}

void CameraPan::Update(float aTime)
{
	if (IsActive(aTime))
	{
		float progress = GetProgress(aTime);
		myCamera->SetPosition(myPath.Evaluate(progress));
	}
}

void CameraPan::SetControlPoints(V3F aC1, V3F aC2)
{
	myPath.SetControlPoints(aC1, aC2);
}

void CameraPan::SetPath(CinematicPath aPath)
{
	myPath = aPath;
}

void CameraPan::AppendTo(FiskJSON::Object& aObject)
{
	FiskJSON::Object* me = new FiskJSON::Object();
	AppendCommon(*me);
	FiskJSON::Object* path = new FiskJSON::Object();
	myPath.AppendTo(path);
	me->AddChild("Path", path);

	me->AddValueChild("CustomName", std::string(myName));

	aObject.PushChild(me);
}

void CameraPan::Edit()
{
	myPath.Edit();
}

const char* CameraPan::GetType()
{
	return "cameraPan";
}
