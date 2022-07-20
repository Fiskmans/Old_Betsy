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
		ImDrawList* drawlist = ImGui::GetWindowDrawList();

		AssetHandle handle = myTexture.GetOutStorage(aId)->template As<AssetHandle>();

		ImVec2 size = ImVec2(40.f * aScale, 40.f * aScale);
		ImVec2 bottomRight(aTopLeft.x + size.x, aTopLeft.y + size.y);

		if (!handle.IsValid())
		{
			drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale * 0.8f, ImVec2(aTopLeft.x + 3.f * aScale, aTopLeft.y + 16.f * aScale), ImColor(1.f, 1.f, 1.f, 1.f), "No Tex");
			drawlist->AddRect(aTopLeft, bottomRight, ImColor(1.f, 1.f, 1.f, 1.f));
			return;
		}

		ImGui::SetCursorScreenPos(aTopLeft);

		const float whiteIntensity = 0.8f;
		const float greyIntensity = 0.6f;

		const ImColor whiteColor = ImColor(whiteIntensity, whiteIntensity, whiteIntensity, 1.f);
		const ImColor greycolor = ImColor(greyIntensity, greyIntensity, greyIntensity, 1.f);

		ImGui::PushClipRect(aTopLeft, bottomRight, true);
		const float gridSize = 10.f;
		
		bool xParity = true;
		bool yParity = true;

		for (float x = aTopLeft.x - gridSize; x < bottomRight.x; x += gridSize)
		{
			xParity = !xParity;
			yParity = true;
			for (float y = aTopLeft.y - gridSize; y < bottomRight.y; y += gridSize)
			{
				yParity = !yParity;

				drawlist->AddRectFilled(ImVec2(x, y), ImVec2(x + gridSize, y + gridSize), (xParity ^ yParity) ? whiteColor : greycolor);
			}
		}
		ImGui::PopClipRect();

		ImGui::Image(handle.Get<TextureAsset>().myTexture, size, ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 0.4f));
		drawlist->AddRect(aTopLeft, bottomRight, ImColor(1.f, 1.f, 1.f, 1.f));
	}

}
