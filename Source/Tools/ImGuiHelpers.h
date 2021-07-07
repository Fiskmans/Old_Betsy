#pragma once

class FullscreenRenderer;
class FullscreenTextureFactory;

namespace Tools
{
	namespace ImguiHelperGlobals
	{
		void ResetCounter();
		void Setup(ID3D11Device* aDevice, ID3D11DeviceContext* aContext, FullscreenRenderer* aRenderer, FullscreenTextureFactory* aFactory);
	}
	extern thread_local bool dummy;

	struct TimeTree;
	void ZoomableImGuiImage(void* aTexture,ImVec2 aSize);

	void ZoomableImGuiSnapshot(void* aTexture, ImVec2 aSize);


	bool EditPosition(const char* aName, float* aData, bool& hovered = dummy);

	std::string PadOrTrimTo(const std::string& aString, int aLength);

	void DrawTimeTree(Tools::TimeTree* aTree);
}

