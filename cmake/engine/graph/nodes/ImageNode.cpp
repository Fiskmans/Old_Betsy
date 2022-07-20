#include "engine/graph/nodes/ImageNode.h"

#include "engine/assets/AssetManager.h"

namespace engine::graph::node
{
	void ImageNode::Activate(NodeInstanceId aId)
	{
		myTexture.Write(aId, AssetManager::GetInstance().GetTexture(myFilePath.Get(aId), true));
	}

	void ImageNode::Imgui(NodeInstanceId aId, float aScale, ImVec2 aTopLeft)
	{
		ImGui::SetCursorScreenPos(aTopLeft);
		ImDrawList* drawlist = ImGui::GetWindowDrawList();

		AssetHandle handle = myTexture.GetOutStorage(aId)->template As<AssetHandle>();

		if (!handle.IsValid())
		{
			drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, ImVec2(aTopLeft.x + 5.f*aScale, aTopLeft.y + 16.f * aScale), ImColor(1.f, 1.f, 1.f, 1.f), "No Tex");
			return;
		}

		ImGui::Image(handle.Get<TextureAsset>().myTexture, ImVec2(40.f * aScale, 40.f * aScale), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 0.4f));
	}

}
