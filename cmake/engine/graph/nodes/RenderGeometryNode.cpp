#include "engine/graph/nodes/RenderGeometryNode.h"

#include "engine/graphics/RenderManager.h"

#include "engine/assets/AssetManager.h"

namespace engine::graph::nodes
{
	void RenderGeometryNode::Activate()
	{
		AssetHandle<GBufferAsset>& stashed = myOutGeometry.GetOutStorage()->template As<AssetHandle<GBufferAsset>>();
		if (!stashed.IsValid() || myInResolution.GetInPinInstance()->IsDirty())
		{
			tools::V2ui resolution = myInResolution;
			if (resolution == tools::V2ui(0, 0))
				resolution = WindowManager::GetInstance().GetSize(); //TODO [1]: register for resolution changes

			stashed = AssetManager::GetInstance().MakeGBuffer(resolution);
		}
		
		AssetHandle<DepthTextureAsset> depth;
		AssetHandle<DepthTextureAsset>& depthStashed = myStoredDepth;
		AssetHandle<DepthTextureAsset>& depthIn = myInDepth;
		if (depthIn.IsValid())
		{
			depth = depthIn;
		}
		else
		{
			if (!depthStashed.IsValid())
			{
				if (myInResolution != tools::V2ui(0, 0))
				{
					depthStashed = AssetManager::GetInstance().MakeDepthTexture(myInResolution);
				}
				else
				{
					myOutGeometry.GetOutStorage()->MarkRefreshed();
					return;
				}
			}
			depth = depthStashed;
		}

		graphics::RenderManager::GetInstance().MapTextures(stashed, {}, depth);
		graphics::RenderManager::GetInstance().GetDeferredRenderer().GenerateGBuffer(myInCamera);
		myOutGeometry = stashed;
		myOutDepth = depth;
	}

	void RenderGeometryNode::Imgui(float aScale, ImVec2 aTopLeft)
	{
		ImGui::SetCursorScreenPos(aTopLeft);
		ImDrawList* drawlist = ImGui::GetWindowDrawList();

		AssetHandle<GBufferAsset>& handle = myOutGeometry.GetOutStorage()->template As<AssetHandle<GBufferAsset>>();

		if (!handle.IsValid())
		{
			drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, ImVec2(aTopLeft.x + 5.f * aScale, aTopLeft.y + 16.f * aScale), ImColor(1.f, 1.f, 1.f, 1.f), "No Tex");
			return;
		}

		handle.Access().myGBuffer.Imgui(aTopLeft, ImVec2(aTopLeft.x + 80 * aScale, aTopLeft.y + 160 * aScale));
	}
}