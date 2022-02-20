#pragma once

#include <WindowControl.h>
#include "..//IMGui/imgui_ne/bin/ThirdParty/imgui/imgui_CubicBezierWidget.h"
#include "../IMGui/imgui_ne/NodeEditor/Source/imgui_bezier_math.h"

#ifdef USEIMGUI
WindowControl::Window("PlayerCurves", [&]()
{
	ImGui::Bezier("Run Acceleration", myRunAccCurve);
});
#endif

float GetValueOnCurve(float start, float end, float curve[5], float t)
{
	return LERP(start, end, ImGui::BezierValue(t, curve));
}