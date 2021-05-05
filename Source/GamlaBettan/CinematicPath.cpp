#include <pch.h>
#include "CinematicPath.h"
#include <Bezier.h>

CinematicPath::CinematicPath()
{
	myIsBezier = false;
	myCanEditStart = false;
}

CinematicPath::CinematicPath(V3F aEnd)
{
	myIsBezier = false;
	myCanEditStart = false;
	myEnd = aEnd;
}

void CinematicPath::SetStart(V3F aStart)
{
	myStart = aStart;
}

void CinematicPath::SetControlPoints(V3F aC1, V3F aC2)
{
	myIsBezier = true;
	myC1 = aC1;
	myC2 = aC2;
}

V3F CinematicPath::Evaluate(float aTime)
{
	if (myIsBezier)
	{
		return Math::BezierInterpolation<V3F,4>({ myStart,myC1,myC2,myEnd }, aTime);
	}
	return LERP(myStart,myEnd,aTime);
}

void CinematicPath::AppendTo(FiskJSON::Object* aObject)
{
	FiskJSON::Object* to = new FiskJSON::Object();
	to->MakeArray();
	for (auto& val : myEnd)
	{
		to->PushValueChild(val);
	}
	if (myCanEditStart)
	{

		FiskJSON::Object* start = new FiskJSON::Object();
		start->MakeArray();
		for (auto& val : myStart)
		{
			start->PushValueChild(val);
		}
		aObject->AddChild("From", start);
	}
	aObject->AddChild("To", to);
	if (myIsBezier)
	{
		FiskJSON::Object* c1 = new FiskJSON::Object();
		c1->MakeArray();
		for (auto& val : myC1)
		{
			c1->PushValueChild(val);
		}
		aObject->AddChild("Control1", c1);

		FiskJSON::Object* c2 = new FiskJSON::Object();
		c2->MakeArray();
		for (auto& val : myC2)
		{
			c2->PushValueChild(val);
		}
		aObject->AddChild("Control2", c2);
	}
	
}

void CinematicPath::ParseFrom(FiskJSON::Object& aObject)
{
	const auto ReadVector = [](FiskJSON::Object& aObject) -> V3F
	{
		if (aObject.Is<FiskJSON::Array>())
		{
			float data[3];
			char count = 0;
			for (auto& arrChild : aObject.Get<FiskJSON::Array>())
			{
				if (arrChild->GetIf(data[count]))
				{
					if (++count == 3)
					{
						return V3F(data[0], data[1], data[2]);
					}
				}
			}
		}
		return V3F(0, 0, 0);
	};
	myStart = ReadVector(aObject["From"]);
	if (myStart != V3F(0,0,0))
	{
		myCanEditStart = true;
	}
	myEnd = ReadVector(aObject["To"]);
	myC1 = ReadVector(aObject["Control1"]);
	myC2 = ReadVector(aObject["Control2"]);
	if (myC1 != V3F(0, 0, 0) && myC2 != V3F(0, 0, 0))
	{
		myIsBezier = true;
	}
}

void CinematicPath::Edit(V3F* aCurrent)
{
#if USEIMGUI
	bool shouldShowPath = false;
	if (myCanEditStart)
	{
		shouldShowPath |= Tools::EditPosition("Start", myStart,aCurrent,"Current");
	}
	shouldShowPath |= Tools::EditPosition("Target", myEnd, aCurrent, "Current");
	
	ImGui::Checkbox("Bezier",&myIsBezier);
	if (myIsBezier)
	{
		shouldShowPath |= Tools::EditPosition("Control 1", myC1, aCurrent, "Current");
		shouldShowPath |= Tools::EditPosition("Control 2", myC2, aCurrent, "Current");
	}
	if (shouldShowPath)
	{
		VisualizePath();
	}
#endif
}

void CinematicPath::AllowEditStart()
{
	myCanEditStart = true;
}

void CinematicPath::VisualizePath()
{
	if (myIsBezier)
	{
		std::vector<V3F> points;
		for (size_t i = 0; i < 31; i++)
		{
			points.push_back(Math::BezierInterpolation<V3F, 4>({ myStart,myC1,myC2,myEnd }, float(i) / 30.f));
		}
		DebugDrawer::GetInstance().DrawLines(points);
		DebugDrawer::GetInstance().DrawLines({ myStart,myC1,myC2,myEnd });
	}
	else
	{
		DebugDrawer::GetInstance().DrawArrow(myStart, myEnd);
	}
}
