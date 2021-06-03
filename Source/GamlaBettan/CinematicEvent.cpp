#include "pch.h"
#include "CinematicEvent.h"

void CinematicEvent::SetTiming(float aStart, float aDuration)
{
	myStart = aStart;
	myDuration = aDuration;
}




void CinematicEvent::NameBox()
{
#if USEIMGUI
	ImGui::InputText("Name",myName,64);
#endif
}

void CinematicEvent::AppendCommon(FiskJSON::Object& aObject)
{
	aObject.AddValueChild("Type", GetType());
	aObject.AddValueChild("Start", myStart);
	aObject.AddValueChild("Duration", myDuration);
}