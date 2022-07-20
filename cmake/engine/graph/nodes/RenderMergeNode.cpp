#include "engine/graph/nodes/RenderMergeNode.h"

#include "engine/graphics/RenderManager.h"

#include "engine/assets/AssetManager.h"

namespace engine::graph::node
{
	void RenderMergeNode::Activate(NodeInstanceId aId)
	{
		AssetHandle& stashed = myOutTexture.GetOutStorage(aId)->template As<AssetHandle>();
		if (!stashed.IsValid() || myInResolution.GetInPinInstance(aId)->IsDirty() || myInFormat.GetInPinInstance(aId)->IsDirty())
		{
			DXGI_FORMAT format = myInFormat.Get(aId);
			if (format != DXGI_FORMAT_UNKNOWN)
			{
				stashed = AssetManager::GetInstance().MakeTexture(myInResolution.Get(aId), format);
			}
			else
			{
				myOutTexture.GetOutStorage(aId)->MarkRefreshed();
				return;
			}
		}

		std::vector<RenderManager::TextureMapping> mappings;

		AssetHandle first = myInTexture1.Get(aId);
		
		if (first.IsValid())
			mappings.emplace_back(first, 0);

		AssetHandle second = myInTexture2.Get(aId);

		if (second.IsValid())
			mappings.emplace_back(second, 1);

		RenderManager::GetInstance().MapTextures(stashed, mappings);
		RenderManager::GetInstance().GetFullscreenRender().Render(FullscreenRenderer::Shader::MERGE);

		myOutTexture.Write(aId, stashed);
	}

	void RenderMergeNode::Imgui(NodeInstanceId aId, float aScale, ImVec2 aTopLeft)
	{
		ImGui::SetCursorScreenPos(aTopLeft);
		ImDrawList* drawlist = ImGui::GetWindowDrawList();

		AssetHandle handle = myOutTexture.GetOutStorage(aId)->template As<AssetHandle>();

		if (!handle.IsValid())
		{
			drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, ImVec2(aTopLeft.x + 5.f * aScale, aTopLeft.y + 16.f * aScale), ImColor(1.f, 1.f, 1.f, 1.f), "No Tex");
			return;
		}

		ImGui::Image(handle.Get<TextureAsset>().myTexture, ImVec2(40.f * aScale, 40.f * aScale), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 0.4f));
	}
}