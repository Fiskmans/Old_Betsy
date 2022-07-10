#include "engine/graph/Graph.h"

#include "imgui/WindowControl.h"

#include "tools/ImGuiHelpers.h"
#include "tools/Solver.h"
#include "tools/Utility.h"

#include "common/Macros.h"

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



		return capture;
	}

	void Graph::AddLink(OutPinInstanceBase* aFrom, InPinInstance* aTo)
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

	NodeInstance::NodeInstance(BuiltNode& aType, ImVec2 aPosition)
		: DrawablePinBlock(aPosition)
		, myType(&aType)
		, myId(GetNextNodeInstanceId())
	{
		myType->AddInstance(myId);
	}


	bool NodeInstance::Imgui(Graph* aGraph, float aScale, ImVec2 aPosition)
	{
		return DrawablePinBlock::Imgui(NodeManager::PrettyfyName(myType->Name()).c_str(), aGraph, myId, aScale, aPosition, myType->InPins(), myType->OutPins());
	}

	bool DrawablePinBlock::Imgui(const char* aName, Graph* aGraph, NodeInstanceId aId, float aScale, ImVec2 aPosition, const std::vector<PinBase*>& aInPins, const std::vector<PinBase*>& aOutPins)
	{

		ImGui::PushID(this);

		ImVec2 offset = ImGui::GetWindowPos();
		ImDrawList* drawlist = ImGui::GetWindowDrawList();

		const ImVec2 shadowOffset = ImVec2(6 * aScale, 6 * aScale);

		size_t pinCount = (std::max)(aInPins.size(), aOutPins.size());

		const ImVec2 pinSize = ImVec2(22 * aScale, 16 * aScale);
		const float		headerSize = 20 * aScale;
		const float		pinSpacing = 4 * aScale;

		ImVec2 size = ImVec2(160 * aScale, headerSize + pinSize.y * pinCount + pinSpacing * (pinCount + 1));
		ImVec2 topLeft = ImVec2((myPosition.x + aPosition.x) * aScale + offset.x, (myPosition.y + aPosition.y) * aScale + offset.y);
		ImVec2 bottomRight = ImVec2(topLeft.x + size.x, topLeft.y + size.y);

		ImGui::SetCursorScreenPos(topLeft);
		ImGui::InvisibleButton("heading", ImVec2(size.x, headerSize));

		bool headerHovered = ImGui::IsItemHovered();
		bool headerActive = ImGui::IsItemActivated();

		ImGuiIO& io = ImGui::GetIO();
		if (headerHovered && io.MouseClicked[0] && !io.KeysDown[VK_SPACE])
			myIsMoving = true;

		if (io.MouseReleased[0])
			myIsMoving = false;

		if (myIsMoving)
			myPosition = ImVec2(myPosition.x + io.MouseDelta.x / aScale, myPosition.y + io.MouseDelta.y / aScale);


		drawlist->AddRectFilled(ImVec2(topLeft.x + shadowOffset.x, topLeft.y + shadowOffset.y), ImVec2(bottomRight.x + shadowOffset.x, bottomRight.y + shadowOffset.y), tools::GetImColor(ImGuiCol_BorderShadow), 3.f);
		drawlist->AddRectFilled(topLeft, bottomRight, tools::GetImColor(ImGuiCol_WindowBg), 3.f);
		drawlist->AddRectFilled(topLeft, ImVec2(bottomRight.x, topLeft.y + headerSize), tools::GetImColor(headerActive ? ImGuiCol_HeaderActive : (headerHovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header)), 3.f);
		drawlist->AddRect(topLeft, bottomRight, tools::GetImColor(ImGuiCol_Border), 3.f);


		ImVec2 textPos = ImVec2(topLeft.x + 4 * aScale, topLeft.y + 4 * aScale);

		drawlist->AddText(ImGui::GetFont(), ImGui::GetFontSize() * aScale, textPos, tools::GetImColor(ImGuiCol_Text), aName);

		{
			ImVec2 pinPosition = ImVec2(topLeft.x, topLeft.y + headerSize + pinSpacing + pinSize.y / 2.f);

			for (PinBase* pin : aInPins)
			{
				pin->ImGui(aGraph, aScale, pinPosition, aId);
				aGraph->AddPinLocation(pin->GetInPinInstance(aId), pinPosition);
				pinPosition = ImVec2(pinPosition.x, pinPosition.y + pinSpacing + pinSize.y);
			}
		}

		{
			ImVec2 pinPosition = ImVec2(bottomRight.x, topLeft.y + headerSize + pinSpacing + pinSize.y / 2.f);

			for (PinBase* pin : aOutPins)
			{
				pin->ImGui(aGraph, aScale, pinPosition, aId);
				aGraph->AddPinLocation(pin->GetOutPinInstance(aId), pinPosition);
				pinPosition = ImVec2(pinPosition.x, pinPosition.y + pinSpacing + pinSize.y);
			}
		}

		ImGui::PopID();
		return false;// TODO capture
	}

	bool GraphExportPinBlock::Imgui(Graph* aGraph, float aScale, ImVec2 aPosition)
	{
		return DrawablePinBlock::Imgui("Export", aGraph, InvalidNodeInstanceId, aScale, aPosition, myPins, {});
	}

	bool GraphImportPinBlock::Imgui(Graph* aGraph, float aScale, ImVec2 aPosition)
	{
		return DrawablePinBlock::Imgui("Import", aGraph, InvalidNodeInstanceId, aScale, aPosition, {}, myPins);
	}

	PinLink::PinLink(OutPinInstanceBase* aFrom, InPinInstance* aTo)
		: myFrom(aFrom)
		, myTo(aTo)
	{
		aFrom->GetStorage().AddDependent(aTo);
	}

	PinLink::~PinLink()
	{
		myFrom->GetStorage().RemoveDependent(myTo);
	}

	void PinLink::Imgui(Graph* aGraph, const std::unordered_map<PinInstanceBase*, ImVec2>& aLocations, float aScale, ImVec2 aPosition)
	{
		const float bendyness = 50.f * aScale;

		ImVec2 start = aLocations.find(myFrom)->second;
		ImVec2 q1 = ImVec2(start.x + bendyness, start.y);

		ImVec2 end = aLocations.find(myTo)->second;
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

}