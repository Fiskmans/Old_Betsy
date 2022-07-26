#include "engine/graph/Graph.h"

#include "engine/graph/Node.h"
#include "engine/graph/NodeInstance.h"

#include "imgui/WindowControl.h"

#include "tools/ImGuiHelpers.h"
#include "tools/Solver.h"
#include "tools/Utility.h"

#include "common/Macros.h"

#include <algorithm>

#define NOMINMAX
#include <windows.h>

namespace engine::graph 
{
	Graph::Graph(const std::string& aName, const std::vector<PinBase*>& aExportPins, const std::vector<PinBase*>& aImportPins)
		: myName(aName)
	{
		GraphManager::GetInstance().AddGraph(this);
		if (!aExportPins.empty())
			myExportBlock = GraphExportPinBlock(aExportPins);
		if (!aImportPins.empty())
			myImportBlock = GraphImportPinBlock(aImportPins);
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

		myPinPositions.clear();

		for (const std::unique_ptr<NodeInstance>& node : myNodes)
			capture |= node->Imgui(this, aScale, aPosition);

		if (myExportBlock)
			capture |= myExportBlock->Imgui(this, aScale, aPosition);

		if (myImportBlock)
			capture |= myImportBlock->Imgui(this, aScale, aPosition);

		for (std::unique_ptr<PinLink>& link : myLinks)
			link->Imgui(this, myPinPositions, aScale, aPosition);

		if (myLinkToRemove)
		{
			decltype(myLinks)::iterator it = myLinks.begin();
			while (it != myLinks.end())
			{
				if (it->get() == myLinkToRemove)
				{
					myLinks.erase(it);
					break;
				}
				it++;
			}

			myLinkToRemove = nullptr;
		}

		NodeManager::GetInstance().ImguiAddNodes(this, aScale, aPosition);

		if (!capture && ImGui::GetIO().MouseClicked[0] && !ImGui::GetIO().KeysDown[VK_SPACE])
			mySelection.StartSelecting(ImGui::GetMousePos());

		if (ImGui::GetIO().MouseReleased[0])
			mySelection.FinalizeSelection();

		if(mySelection.IsSelecting())
		{
			mySelection.SetEnd(ImGui::GetMousePos());
			mySelection.UpdateSelection();
		}

		mySelection.DrawSelection();

		if (ImGui::GetIO().KeysDown[VK_DELETE] || ImGui::GetIO().KeysDown[VK_BACK])
		{
			RemoveNodes(mySelection.All());
			mySelection.InvalidateSelection();
		}

		return capture;
	}

	void Graph::AddLink(OutPinInstanceBase* aFrom, InPinInstanceBase* aTo)
	{
		aTo->myTarget = &aFrom->GetStorage();

		myLinks.emplace_back(std::make_unique<PinLink>(aFrom, aTo));
	}

	void Graph::RemoveLink(PinLink* aLink)
	{
		myLinkToRemove = aLink;
	}

	void Graph::AddPinLocation(PinInstanceBase* aPin, ImVec2 aScreenPosition)
	{
		myPinPositions.emplace(aPin, aScreenPosition);
	}

	Selection& Graph::GetSelection()
	{
		return mySelection;
	}

	const std::string& Graph::Name()
	{
		return myName;
	}

	void Graph::RemoveNodes(std::unordered_set<DrawablePinBlock*>& aSelection)
	{

		for (DrawablePinBlock* item : aSelection)
		{
			decltype(myNodes)::iterator it = std::find_if(std::begin(myNodes), std::end(myNodes), [item](const std::unique_ptr<NodeInstance>& aLHs) { return aLHs.get() == item; });

			if (it != std::end(myNodes))
			{
				(*it)->RemoveAllRelatedLinks(myLinks);
				myNodes.erase(it);
			}
		}
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
		PinBase::UpdateImGui();

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
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar,
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

		if (ImGui::GetIO().KeysDown[VK_SPACE] && !selection->GetSelection().IsSelecting())
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
		const float gridParalax = 0.83f;

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

	bool GraphExportPinBlock::Imgui(Graph* aGraph, float aScale, ImVec2 aPosition)
	{
		return DrawablePinBlock::Imgui("Export", aGraph, aScale, aPosition, myPins, {});
	}

	bool GraphImportPinBlock::Imgui(Graph* aGraph, float aScale, ImVec2 aPosition)
	{
		return DrawablePinBlock::Imgui("Import", aGraph, aScale, aPosition, {}, myPins);
	}

	PinLink::PinLink(OutPinInstanceBase* aFrom, InPinInstanceBase* aTo)
		: myFrom(aFrom)
		, myTo(aTo)
	{
		aTo->LinkTo(aFrom);
	}

	PinLink::~PinLink()
	{
		myTo->UnlinkFrom(myFrom);
		myFrom->GetStorage().RemoveDependent(myTo);
	}

	void PinLink::Imgui(Graph* aGraph, const std::unordered_map<PinInstanceBase*, ImVec2>& aLocations, float aScale, ImVec2 aPosition)
	{
		ImVec2 start = aLocations.find(myFrom)->second;
		ImVec2 end = aLocations.find(myTo)->second;

		const float bendyness = (std::min)(50.f * aScale, std::abs(start.x - end.x) / 2.f);

		ImVec2 q1 = ImVec2(start.x + bendyness, start.y);
		ImVec2 q2 = ImVec2(end.x - bendyness, end.y);

		ImDrawList* drawlist = ImGui::GetWindowDrawList();


		ImVec2 mousePos = ImGui::GetMousePos();

		auto pointAt = [&](float aValue) -> ImVec2
		{
			float x[4] = { start.x, q1.x, q2.x, end.x };
			float y[4] = { start.y, q1.y, q2.y, end.y };

			for (size_t depth = 3; depth > 0; depth--)
			{
				for (size_t i = 0; i < depth; i++)
				{
					x[i] = LERP(x[i], x[i + 1], aValue);
					y[i] = LERP(y[i], y[i + 1], aValue);
				}
			}

			return ImVec2(x[0], y[0]);
		};

		auto function = [&](float aValue) -> float
		{
			ImVec2 point = pointAt(aValue);
			return tools::Square(point.x - mousePos.x) + tools::Square(point.y - mousePos.y);
		};

		float closest = tools::FindMinimum(function, 0.f, 1.f, 50, 1.e-5f);

		float squareDistance = function(closest);

		bool hovered = squareDistance < tools::Square(2.f * aScale * 3.f);
		
		const ImColor normalColor = ImColor(0.6f, 0.6f, 0.6f, 0.8f);
		const ImColor hoveredColor = ImColor(0.8f, 0.8f, 0.8f, 1.f);

		drawlist->AddBezierCurve(start, q1, q2, end, hovered ? hoveredColor : normalColor, 2.f * aScale);

		if (ImGui::GetIO().MouseClicked[2] && hovered)
			aGraph->RemoveLink(this);
	}

	Selection::Selection()
		: myIsSelecting(false)
	{
	}

	bool Selection::IsSelecting()
	{
		return myIsSelecting;
	}

	bool Selection::Intersects(ImVec2 aTopLeft, ImVec2 aBottomRight)
	{
		ImVec2 topLeft = TopLeft();
		ImVec2 bottomRight = BottomRight();

		if (topLeft.x < aBottomRight.x &&
			bottomRight.x > aTopLeft.x &&
			topLeft.y < aBottomRight.y &&
			bottomRight.y > aTopLeft.y)	
		{
			return true;
		}

		return false;
	}

	ImVec2 Selection::TopLeft()
	{
		return ImVec2((std::min)(mySelectionStart.x, mySelectionEnd.x),(std::min)(mySelectionStart.y, mySelectionEnd.y));
	}

	ImVec2 Selection::BottomRight()
	{
		return ImVec2((std::max)(mySelectionStart.x, mySelectionEnd.x), (std::max)(mySelectionStart.y, mySelectionEnd.y));
	}

	void Selection::DrawSelection()
	{
		if (!myIsSelecting)
			return;

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddRect(TopLeft(), BottomRight(), ImColor(0.8f, 0.8f, 0.9f, 0.8f), 4);
	}

	void Selection::AddToSelection(DrawablePinBlock* aPinBlock)
	{
		myNextSelected.insert(aPinBlock);
	}

	bool Selection::IsSelected(DrawablePinBlock* aPinBlock)
	{
		return mySelected.contains(aPinBlock);
	}

	void Selection::StartSelecting(ImVec2 aSceenPos)
	{
		myIsSelecting = true;

		mySelectionStart = aSceenPos;
		mySelectionEnd = aSceenPos;
	}

	void Selection::SetEnd(ImVec2 aScreenPos)
	{
		mySelectionEnd = aScreenPos;
	}

	void Selection::FinalizeSelection()
	{
		if (myIsSelecting)
			UpdateSelection();
			
		myIsSelecting = false;
	}

	void Selection::InvalidateSelection()
	{
		mySelected = {};
	}

	void Selection::UpdateSelection()
	{
		mySelected = myNextSelected;
		myNextSelected = {};
	}

	void Selection::InvalidateIfOnly(DrawablePinBlock* aPinBlock)
	{
		if (mySelected.size() != 1)
			return;

		if (!IsSelected(aPinBlock))
			return;

		InvalidateSelection();
	}

	void Selection::MoveAll(ImVec2 aDelta)
	{
		for (DrawablePinBlock* pinBlock : mySelected)
			pinBlock->Move(aDelta);
	}
}