#ifndef ENGINE_UTILITIES_IMGUI_THREE_AXIS_CONTROL_H
#define ENGINE_UTILITIES_IMGUI_THREE_AXIS_CONTROL_H

#include "imgui/imgui.h"

#include "tools/MathVector.h"

namespace engine::utilities
{
	tools::V3f ImguiThreeAxisControl(const char* aId, ImVec2 aSize = ImVec2(0.f, 0.f));
}

#endif