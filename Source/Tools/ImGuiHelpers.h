#pragma once
#include "imgui.h"
#include "Vector.hpp"

class FullscreenRenderer;
class FullscreenTextureFactory;

namespace Tools
{
	namespace ImguiHelperGlobals
	{
		void ResetCounter();
		void Setup(ID3D11Device* aDevice, ID3D11DeviceContext* aContext, FullscreenRenderer* aRenderer, FullscreenTextureFactory* aFactory);
	}


	struct TimeTree;
	void ZoomableImGuiImage(void* aTexture,ImVec2 aSize);

	void ZoomableImGuiSnapshot(void* aTexture, ImVec2 aSize);

	bool EditPosition(const char* aName, V3F& aVector, V3F* aAdditionalInput = nullptr, const char* aAdditionalInputName = nullptr);

	std::string PadOrTrimTo(const std::string& aString, int aLength);

	void DrawTimeTree(Tools::TimeTree* aTree);
}

