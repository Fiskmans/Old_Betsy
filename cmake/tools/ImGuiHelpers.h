
#ifndef IMGUI_IMGUI_HELPERS_H
#define IMGUI_IMGUI_HELPERS_H

#include "imgui/imgui.h"
#include <string>

#include <d3d11.h>

namespace tools
{
	namespace ImguiHelperGlobals
	{
		void ResetCounter();
	}
	extern thread_local bool dummy;

	struct TimeTree;
	void ZoomableImGuiImage(void* aTexture, ImVec2 aSize);

	void ZoomableImGuiSnapshot(void* aTexture, ImVec2 aSize);


	bool EditPosition(const char* aName, float* aData, bool& hovered = dummy);

	std::string PadOrTrimTo(const std::string& aString, int aLength);

	void DrawTimeTree(tools::TimeTree* aTree);
}

#endif