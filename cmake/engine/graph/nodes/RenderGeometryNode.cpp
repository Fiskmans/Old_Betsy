#include "engine/graph/nodes/RenderGeometryNode.h"

#include "engine/graphics/RenderManager.h"

#include "engine/assets/AssetManager.h"

namespace engine::graph::nodes
{
	void RenderGeometryNode::Activate()
	{
		graphics::GBuffer& stashed = myOutGeometry.GetOutStorage()->template As<graphics::GBuffer>();
		if (!stashed.IsValid() || myInResolution.GetInPinInstance()->IsDirty())
		{
			if (myInResolution != tools::V2ui(0, 0))
			{
				stashed = AssetManager::GetInstance().MakeGBuffer(myInResolution);
			}
			else
			{
				myOutGeometry.GetOutStorage()->MarkRefreshed();
				return;
			}
		}
		
		graphics::DepthTexture depth;
		graphics::DepthTexture& depthStashed = myStoredDepth;
		graphics::DepthTexture& depthIn = myInDepth;
		if (depthIn.IsValid())
		{
			if (depthStashed.IsValid())
				depthStashed.Release();
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

		graphics::RenderManager::GetInstance().MapTextures(stashed, {}, &depth);
		graphics::RenderManager::GetInstance().GetDeferredRenderer().GenerateGBuffer(myInCamera, myInModels);
		myOutGeometry = stashed;
		myOutDepth = depth;
	}

	void RenderGeometryNode::Imgui(float aScale, ImVec2 aTopLeft)
	{
		ImGui::SetCursorScreenPos(aTopLeft);
		ImDrawList* drawlist = ImGui::GetWindowDrawList();

		graphics::GBuffer& handle = myOutGeometry.GetOutStorage()->template As<graphics::GBuffer>();

		if (!handle.IsValid())
		{
			drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, ImVec2(aTopLeft.x + 5.f * aScale, aTopLeft.y + 16.f * aScale), ImColor(1.f, 1.f, 1.f, 1.f), "No Tex");
			return;
		}

		handle.Imgui(aTopLeft, ImVec2(aTopLeft.x + 80 * aScale, aTopLeft.y + 160 * aScale));
	}
}