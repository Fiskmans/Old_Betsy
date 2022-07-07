#include "engine/graph/Graph.h"

#include "imgui/WindowControl.h"


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

	void Graph::Imgui()
	{
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
		static ImVec2 windowPosition = ImVec2(0,0);
		old_betsy_imgui::WindowControl::Window("Graph manager",
			[&]()
		{
			if (ManageGraphs())
				return;

			static ImVec2 lastMousePos = ImVec2(0, 0);
			ImVec2 mousePos = ImGui::GetMousePos();
			static bool held = false;
			if (!held && !ImGui::IsWindowHovered())
			{
				lastMousePos = mousePos;
				return;
			}

			held = ImGui::GetIO().MouseDown[0];
			if (held)
			{
				windowPosition = ImVec2(
					windowPosition.x + (mousePos.x - lastMousePos.x),
					windowPosition.y + (mousePos.y - lastMousePos.y)
				);
			}

			lastMousePos = mousePos;
		}, 
		ImGuiWindowFlags_NoMove,
			[&]()
		{
			ImGui::SetNextWindowPos(windowPosition, ImGuiCond_Always);
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

		ImGui::PushClipRect(topLeft, bottomRight, true);
		drawList->AddRectFilled(topLeft, bottomRight, ImColor(0.4f, 0.4f, 0.4f, 1.f), 1.f);


		selection->Imgui();

		ImGui::PopClipRect();
		ImVec2 mousepos = ImGui::GetMousePos();

		return mousepos.x > topLeft.x && mousepos.x < bottomRight.x
			&& mousepos.y > topLeft.y && mousepos.y < bottomRight.y;
	}

}