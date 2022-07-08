#include "engine/graph/Graph.h"

#include "imgui/WindowControl.h"

#include "tools/ImGuiHelpers.h"

#define NOMINMAX
#include <windows.h>

namespace engine::graph 
{
	Graph::Graph(const std::string& aName)
		: myName(aName)
	{
		GraphManager::GetInstance().AddGraph(this);
	}

	Graph::~Graph()
	{
		GraphManager::GetInstance().RemoveGraph(this);
	}

	void Graph::AddNode(BuiltNode& aType, ImVec2 aPosition)
	{
		myNodes.emplace_back(std::make_unique<NodeInstance>(aType, aPosition));
	}

	bool Graph::Imgui(float aScale, ImVec2 aPosition)
	{
		bool capture = false;

		for (const std::unique_ptr<NodeInstance>& node : myNodes)
			capture |= node->Imgui(aScale, aPosition);

		return capture;
	}

	const std::string& Graph::Name()
	{
		return myName;
	}

	void GraphManager::AddGraph(Graph* aGraph)
	{
		myGraphs.push_back(aGraph);
	}

	void GraphManager::RemoveGraph(Graph* aGraph)
	{
		myGraphs.erase(std::find(myGraphs.begin(), myGraphs.end(), aGraph));
	}

	void GraphManager::Imgui()
	{
		PinBase::Imgui();

		static ImVec2 windowPosition = ImVec2(0,0);
		static ImVec2 newWindowPosition = ImVec2(0,0);
		old_betsy_imgui::WindowControl::Window("Graph manager",
			[&]()
		{
			if (ManageGraphs())
				return;

			if (ImGui::IsAnyItemHovered())
				return;

			ImVec2 delta = ImGui::GetMouseDragDelta();
			newWindowPosition = ImVec2(windowPosition.x + delta.x, windowPosition.y + delta.y);

			if (!ImGui::GetIO().MouseDown[0])
				windowPosition = newWindowPosition;
		}, 
		ImGuiWindowFlags_NoMove,
			[&]()
		{
			ImGui::SetNextWindowPos(newWindowPosition, ImGuiCond_Always);
		});
	}

	bool GraphManager::ManageGraphs()
	{
		static Graph* selection = nullptr;
		bool found = false;

		for (Graph* graph : myGraphs)
		{
			found |= graph == selection;
		}

		if (!found)
			selection = nullptr;

		if (ImGui::BeginCombo("Graph", selection ? selection->Name().c_str() : "null"))
		{
			for (Graph* graph : myGraphs)
				if (ImGui::Selectable(graph->Name().c_str(), selection == graph))
					selection = graph;
			ImGui::EndCombo();
		}

		if (selection == nullptr)
			return false;

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 padding = ImGui::GetStyle().WindowPadding;

		ImVec2 topLeft = ImGui::GetCursorScreenPos();
		ImVec2 bottomRight = ImVec2(windowPos.x + windowSize.x - padding.x, windowPos.y + windowSize.y - padding.y);


		ImDrawList* drawList = ImGui::GetWindowDrawList();

		static float scale = 1.f;
		static ImVec2 position = ImVec2(0, 0);
		static ImVec2 newPosition = ImVec2(0, 0);

		float wheelDelta = ImGui::GetIO().MouseWheel;
		if (wheelDelta != 0)
			scale *= pow(0.9f, -wheelDelta);

		if (ImGui::GetIO().KeysDown[VK_SPACE])
		{
			ImVec2 delta	= ImGui::GetMouseDragDelta();
			newPosition		= ImVec2(position.x + delta.x / scale, position.y + delta.y / scale);
			if (!ImGui::GetIO().MouseDown[0])
				position = newPosition;
		}
		else
		{
			position = newPosition;
		}

		ImGui::PushClipRect(topLeft, bottomRight, true);
		drawList->AddRectFilled(topLeft, bottomRight, ImColor(0.2f, 0.2f, 0.2f, 1.f), 1.f);

		const float unitsPerline = 30.f * scale;
		const float gridParalax = 0.8f;

		for (float	x   = topLeft.x + fmodf(newPosition.x * gridParalax, unitsPerline); x < bottomRight.x; x += unitsPerline)
			drawList->AddLine(ImVec2(x, topLeft.y), ImVec2(x, bottomRight.y), ImColor(1.f, 1.f, 1.f, 0.2f));

		for (float	y   = topLeft.y + fmodf(newPosition.y * gridParalax, unitsPerline); y < bottomRight.y; y += unitsPerline)
			drawList->AddLine(ImVec2(topLeft.x, y), ImVec2(bottomRight.x, y), ImColor(1.f, 1.f, 1.f, 0.2f));

		selection->Imgui(scale, ImVec2(newPosition.x + ((bottomRight.x - topLeft.x) * 0.5f) / scale,newPosition.y + ((bottomRight.y - topLeft.y) * 0.5f) / scale ));
		

		ImGui::PopClipRect();
		drawList->AddRect(topLeft, bottomRight, tools::GetImColor(ImGuiCol_Border), ImGui::GetStyle().FrameRounding, 0, ImGui::GetStyle().ChildBorderSize);
		ImVec2 mousepos = ImGui::GetMousePos();

		return mousepos.x > topLeft.x && mousepos.x < bottomRight.x
			&& mousepos.y > topLeft.y && mousepos.y < bottomRight.y;
	}

	NodeInstance::NodeInstance(BuiltNode& aType, ImVec2 aPosition)
		: myType(&aType)
		, myPosition(aPosition)
	{

	}


	bool NodeInstance::Imgui(float aScale, ImVec2 aPosition)
	{
		ImVec2 offset = ImGui::GetWindowPos();
		ImDrawList* drawlist = ImGui::GetWindowDrawList();

		const ImVec2 shadowOffset = ImVec2(6 * aScale, 6 * aScale);

		size_t pinCount = (std::max)(myType->InPins().size() , myType->OutPins().size());

		const ImVec2 pinSize = ImVec2(22 * aScale, 16 * aScale);
		const float		headerSize	= 20 * aScale;
		const float		pinSpacing	= 4 * aScale;

		ImVec2 size = ImVec2(160 * aScale,  headerSize + pinSize.y * pinCount + pinSpacing * (pinCount + 1));
		ImVec2 topLeft = ImVec2(aPosition.x * aScale + offset.x, aPosition.y * aScale + offset.y);
		ImVec2 bottomRight = ImVec2(topLeft.x + size.x, topLeft.y + size.y);

		drawlist->AddRectFilled(ImVec2(topLeft.x + shadowOffset.x, topLeft.y + shadowOffset.y), ImVec2(bottomRight.x + shadowOffset.x, bottomRight.y + shadowOffset.y), tools::GetImColor(ImGuiCol_BorderShadow), 3.f);
		drawlist->AddRectFilled(topLeft, bottomRight, tools::GetImColor(ImGuiCol_WindowBg), 3.f);
		drawlist->AddRectFilled(topLeft, ImVec2(bottomRight.x, topLeft.y + headerSize), tools::GetImColor(ImGuiCol_Header), 3.f);
		drawlist->AddRect(topLeft, bottomRight, tools::GetImColor(ImGuiCol_Border), 3.f);

		ImVec2 textPos = ImVec2(topLeft.x + 4 * aScale, topLeft.y + 4 * aScale);

		drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, textPos, tools::GetImColor(ImGuiCol_Text), myType->Name().c_str());

		{
			ImVec2 pinPosition = ImVec2(topLeft.x + 2 * aScale, topLeft.y + headerSize + pinSpacing);

			for (PinBase* pin : myType->InPins())
			{
				pin->Draw(aScale, pinPosition, true);
				pinPosition = ImVec2(pinPosition.x, pinPosition.y + pinSpacing + pinSize.y);
			}
		}

		{
			ImVec2 pinPosition = ImVec2(bottomRight.x - 2 * aScale - pinSize.x, topLeft.y + headerSize + pinSpacing);

			for (PinBase* pin : myType->OutPins())
			{
				pin->Draw(aScale, pinPosition, false);
				pinPosition = ImVec2(pinPosition.x, pinPosition.y + pinSpacing + pinSize.y);
			}
		}


		return false;// TODO capture
	}

}