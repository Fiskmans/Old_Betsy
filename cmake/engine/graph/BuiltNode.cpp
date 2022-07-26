#include "engine/graph/BuiltNode.h"

#include "engine/graph/Node.h"
#include "engine/graph/NodeInstance.h"
#include "engine/graph/InstancedNodeData.h"

#include "tools/Utility.h"

#include "common/Macros.h"

namespace engine::graph
{
	ImColor ColorFromPointer(void* aPointer)
	{
		float value = reinterpret_cast<std::uintptr_t>(aPointer) * 501 % 20000;
		return ImColor(
			0.6f + 0.4f * cos(value),
			0.6f + 0.4f * cos(value / 1.8f),
			0.6f + 0.4f * cos(value / 3.2f),
			1.f);
	}

	BuiltNode::BuiltNode(NodeBase* aBaseNode)
		: myBaseNode(aBaseNode)
	{
	}

	const char* BuiltNode::Name() 
	{ 
		return typeid(*myBaseNode).name(); 
	}

	void BuiltNode::AddInPin(PinBase* aInPin)
	{
		myInPins.push_back(aInPin);
	}

	void BuiltNode::AddOutPin(PinBase* aOutPin)
	{
		myOutPins.push_back(aOutPin);
	}

	size_t BuiltNode::AddData(InstancedNodeDataBase* aData, size_t aSize, size_t aAligment)
	{
		myData.push_back(aData);
		return myMemoryLayoutTemplate.AddItem(aSize, aAligment);
	}

	void BuiltNode::Imgui()
	{
		if (ImGui::TreeNode(Name()))
		{
			ImDrawList* drawlist = ImGui::GetWindowDrawList();

			ImColor defaultCol = ImColor(0.2f, 0.2f, 0.2f, 1.f);
			ImColor borderCol = tools::GetImColor(ImGuiCol_Border);
			ImVec2 size = ImVec2(10, 10);

			auto drawSquare = [size, borderCol, drawlist](ImColor aColor, const char* aName)
			{
				ImVec2 pos = ImGui::GetCursorScreenPos();
				drawlist->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), aColor, 1.f);
				drawlist->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), aColor, 1.f);
				drawlist->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), borderCol, 1.f);
				ImGui::Dummy(size);
				if (ImGui::IsItemHovered())
				{
					ImDrawList* foreground = ImGui::GetForegroundDrawList();
					ImVec2 mp = ImGui::GetMousePos();
					ImVec2 textSize = ImGui::CalcTextSize(aName);
					ImVec2 size = ImVec2(textSize.x + 10.f, textSize.y + 10.f);
					foreground->AddRectFilled(ImVec2(mp.x, mp.y - size.y), ImVec2(mp.x + size.x, mp.y), tools::GetImColor(ImGuiCol_WindowBg));
					foreground->AddRect(ImVec2(mp.x, mp.y - size.y), ImVec2(mp.x + size.x, mp.y), borderCol);
					foreground->AddText(ImVec2(mp.x + 5.f, mp.y - size.y + 5.f), tools::GetImColor(ImGuiCol_Text), aName);
				}
			};

			ImGui::Text("In Pins");
			ImGui::Indent();
			for (PinBase* in : myInPins)
				ImGui::BulletText("%s", in->Name());
			ImGui::Unindent();

			ImGui::Text("Out Pins");
			ImGui::Indent();
			for (PinBase* out : myOutPins)
				ImGui::BulletText("%s", out->Name());
			ImGui::Unindent();

			struct Region
			{
				size_t myStart;
				size_t myEnd;
				ImColor myColor;
				std::string myName;
			};

			std::vector<Region> regions;
			
			ImGui::Text("Data");
			ImGui::Indent();
			for (InstancedNodeDataBase* data : myData)
			{
				Region region;
				region.myStart = data->Offset();
				region.myEnd = region.myStart + data->Size();
				region.myColor = ColorFromPointer(data);
				region.myName = data->Name();
				regions.push_back(region);

				drawSquare(region.myColor, data->Name());
				ImGui::SameLine();
				ImGui::Text("%s",data->Name());
			}
			ImGui::Unindent();

			ImGui::Text("Memory layout");

			size_t left = myMemoryLayoutTemplate.Size();
			size_t at = 0;
			ImGui::Text("Size " PFSIZET, left);

			for (size_t h = 0; h < 4; h++)
			{
				for (size_t y = 0; y < 8; y++)
				{
					for (size_t w = 0; w < 4; w++)
					{
						for (size_t x = 0; x < 8; x++)
						{
							ImColor color = defaultCol;
							std::string name = "Padding";
							for (Region& region : regions)
							{
								if (at >= region.myStart && at < region.myEnd)
								{
									color = region.myColor;
									name = region.myName;
									break;
								}
							}

							drawSquare(color, name.c_str());
							ImGui::SameLine();
							at++;
							left--;
							if (left == 0)
								break;
						}
						ImGui::Dummy(size);
						ImGui::SameLine();
						if (left == 0)
							break;
					}
					ImGui::NewLine();
					if (left == 0)
						break;
				}
				if (left == 0)
					break;
				ImGui::NewLine();
			}

			ImGui::TreePop();
		}
	}

	ImVec2 BuiltNode::ImguiSize()
	{
		return myBaseNode->ImguiSize();
	}

	void BuiltNode::Imgui(float aScale, ImVec2 aTopLeft)
	{
		myBaseNode->Imgui(aScale, aTopLeft);
	}

	void BuiltNode::Construct(NodeInstance* aInstance)
	{
		aInstance->myMemory.resize(myMemoryLayoutTemplate.Size());

		NodeInstanceScopedContext context(aInstance->myMemory.data());

		for (InstancedNodeDataBase* data : myData)
			data->Construct();

		for (PinBase* out : myOutPins)
		{
			out->GetOutStorage()->SetRefreshCallback(std::bind(&NodeInstance::Activate, aInstance));

			Dependable* outInstance = out->GetOutStorage();
			for (PinBase* in : myInPins)
				in->GetInPinInstance()->AddDependent(outInstance);
		}
	}

	void BuiltNode::Destruct(NodeInstance* aInstance)
	{
		NodeInstanceScopedContext context(aInstance->myMemory.data());

		for (InstancedNodeDataBase* data : myData)
			data->Destruct();
	}

	size_t NodeInstanceDataBlobTemplate::AddItem(size_t aSize, size_t aAligment)
	{
		mySize = tools::CeilToMultiple(mySize, aAligment);
		size_t at = mySize;
		mySize += aSize;
		return at;
	}
}