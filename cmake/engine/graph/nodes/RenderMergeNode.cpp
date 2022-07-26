#include "engine/graph/nodes/RenderMergeNode.h"

#include "engine/graphics/RenderManager.h"

#include "engine/assets/AssetManager.h"

namespace engine::graph::node
{
	void RenderMergeNode::Activate()
	{
		AssetHandle& stashed = myOutTexture.GetOutStorage()->template As<AssetHandle>();
		if (!stashed.IsValid() || myInResolution.GetInPinInstance()->IsDirty() || myInFormat.GetInPinInstance()->IsDirty())
		{
			if (myInFormat != DXGI_FORMAT_UNKNOWN && myInResolution != tools::V2ui(0,0))
			{
				stashed = AssetManager::GetInstance().MakeTexture(myInResolution, myInFormat);
			}
			else
			{
				myOutTexture.GetOutStorage()->MarkRefreshed();
				return;
			}
		}

		std::vector<RenderManager::TextureMapping> mappings;

		AssetHandle first = myInTexture1;
		
		if (first.IsValid())
			mappings.emplace_back(first, 0);

		AssetHandle second = myInTexture2;

		if (second.IsValid())
			mappings.emplace_back(second, 1);

		RenderManager::GetInstance().MapTextures(stashed, mappings);
		RenderManager::GetInstance().GetFullscreenRender().Render(FullscreenRenderer::Shader::MERGE);

		myOutTexture.Write(stashed);
	}

	void RenderMergeNode::Imgui(float aScale, ImVec2 aTopLeft)
	{
		ImGui::SetCursorScreenPos(aTopLeft);
		ImDrawList* drawlist = ImGui::GetWindowDrawList();

		AssetHandle handle = myOutTexture.GetOutStorage()->template As<AssetHandle>();

		if (!handle.IsValid())
		{
			drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, ImVec2(aTopLeft.x + 5.f * aScale, aTopLeft.y + 16.f * aScale), ImColor(1.f, 1.f, 1.f, 1.f), "No Tex");
			return;
		}

		ImGui::Image(handle.Get<TextureAsset>().myTexture, ImVec2(40.f * aScale, 40.f * aScale), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 0.4f));
	}
}