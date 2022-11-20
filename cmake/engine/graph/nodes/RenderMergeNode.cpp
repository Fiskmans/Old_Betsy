#include "engine/graph/nodes/RenderMergeNode.h"

#include "engine/graphics/RenderManager.h"

#include "engine/assets/AssetManager.h"

namespace engine::graph::nodes
{
	void RenderMergeNode::Activate()
	{
		AssetHandle<DrawableTextureAsset>& stashed = myOutTexture.GetOutStorage()->template As<AssetHandle<DrawableTextureAsset>>();
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

		std::vector<graphics::RenderManager::TextureMapping> mappings;

		AssetHandle<TextureAsset> first = myInTexture1;
		
		if (first.IsValid())
			mappings.emplace_back(first, 0);

		AssetHandle<TextureAsset> second = myInTexture2;

		if (second.IsValid())
			mappings.emplace_back(second, 1);

		graphics::RenderManager::GetInstance().MapTextures(stashed, mappings);
		graphics::RenderManager::GetInstance().GetFullscreenRender().Render(graphics::FullscreenRenderer::Shader::MERGE);

		myOutTexture.Write(stashed);
	}

	void RenderMergeNode::Imgui(float aScale, ImVec2 aTopLeft)
	{
		ImGui::SetCursorScreenPos(aTopLeft);
		ImDrawList* drawlist = ImGui::GetWindowDrawList();

		AssetHandle<DrawableTextureAsset> handle = myOutTexture.GetOutStorage()->template As<AssetHandle<DrawableTextureAsset>>();

		if (!handle.IsValid())
		{
			drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, ImVec2(aTopLeft.x + 5.f * aScale, aTopLeft.y + 16.f * aScale), ImColor(1.f, 1.f, 1.f, 1.f), "No Tex");
			return;
		}

		ImGui::Image(handle.Access().myTexture, ImVec2(40.f * aScale, 40.f * aScale), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 0.4f));
	}
}