#ifndef ENGINE_GRAPH_GRAPH_H
#define ENGINE_GRAPH_GRAPH_H

#include "tools/Singleton.h"

#include "engine/graph/NodeManager.h"
#include "engine/graph/BuiltNode.h"
#include "engine/graph/NodeInstanceId.h"
#include "engine/graph/DrawablePinBlock.h"

#include "imgui/imgui.h"

#include <vector>
#include <memory>
#include <string>
#include <concepts>
#include <ranges>
#include <optional>
#include <unordered_set>


namespace engine::graph
{

	class Graph;
	class PinLink;


	class GraphExportPinBlock : private DrawablePinBlock
	{
	public:
		GraphExportPinBlock(const std::vector<PinBase*>& aPins)
			: DrawablePinBlock(ImVec2(500,0))
			, myPins(aPins)
		{
		}
		virtual ~GraphExportPinBlock() = default;

		bool Imgui(Graph* aGraph, float aScale, ImVec2 aPosition);
		
	private:
		std::vector<PinBase*> myPins;
	};

	class GraphImportPinBlock : private DrawablePinBlock
	{
	public:
		GraphImportPinBlock(const std::vector<PinBase*>& aPins)
			: DrawablePinBlock(ImVec2(-500, 0))
			, myPins(aPins)
		{
		}
		virtual ~GraphImportPinBlock() = default;

		bool Imgui(Graph* aGraph, float aScale, ImVec2 aPosition);

	private:
		std::vector<PinBase*> myPins;
	};

	class PinLink
	{
	public:
		PinLink(OutPinInstanceBase* aFrom, InPinInstanceBase* aTo);
		~PinLink();

		void Imgui(Graph* aGraph,const std::unordered_map<PinInstanceBase*, ImVec2>& aLocations, float aScale, ImVec2 aPosition);

	private:
		friend NodeInstance;

		OutPinInstanceBase* myFrom;
		InPinInstanceBase*	myTo;
	};

	class Selection
	{
	public:
		Selection();

		bool IsSelecting();

		bool Intersects(ImVec2 aTopLeft, ImVec2 aBottomRight);

		void DrawSelection();
		
		void AddToSelection(DrawablePinBlock* aPinBlock);

		bool IsSelected(DrawablePinBlock* aPinBlock);

		void StartSelecting(ImVec2 aSceenPos);
		void SetEnd(ImVec2 aScreenPos);

		void FinalizeSelection();

		void InvalidateSelection();

		void UpdateSelection();

		void InvalidateIfOnly(DrawablePinBlock* aPinBlock);

		void MoveAll(ImVec2 aDelta);

		inline std::unordered_set<DrawablePinBlock*>& All() { return mySelected; }
	private:

		ImVec2	TopLeft();
		ImVec2	BottomRight();

		bool myIsSelecting;

		ImVec2 mySelectionStart;
		ImVec2 mySelectionEnd;

		std::unordered_set<DrawablePinBlock*> mySelected;
		std::unordered_set<DrawablePinBlock*> myNextSelected;
	};

	class Graph
	{
	public:
		Graph(const std::string& aName, const std::vector<PinBase*>& aExportPins = {}, const std::vector<PinBase*>& aImportPins = {});
		~Graph();

		template<class NodeType>
		void AddNode(ImVec2 aPosition) 
		{ 
			BuiltNode* nodeType = NodeManager::GetInstance().Get<NodeType>();
			if (!nodeType)
				return;

			AddNode(*nodeType, aPosition);
		}

		void AddNode(BuiltNode& aType, ImVec2 aPosition);

		bool Imgui(float aScale, ImVec2 aPosition);

		void AddLink(OutPinInstanceBase* aFrom, InPinInstanceBase* aTo);
		void RemoveLink(PinLink* aLink);

		void AddPinLocation(PinInstanceBase* aPin, ImVec2 aScreenPosition);

		Selection& GetSelection();

		const std::string& Name();
	private:

		void RemoveNodes(std::unordered_set<DrawablePinBlock*>& aSelection);

		PinLink* myLinkToRemove = nullptr;

		std::string myName;
		std::vector<std::unique_ptr<NodeInstance>> myNodes;
		std::vector<std::unique_ptr<PinLink>> myLinks;
		std::optional<GraphExportPinBlock> myExportBlock;
		std::optional<GraphImportPinBlock> myImportBlock;

		std::unordered_map<PinInstanceBase*, ImVec2> myPinPositions;
		Selection mySelection;
	};


	class GraphManager : public fisk::tools::Singleton<GraphManager>
	{
	public:
		void AddGraph(Graph* aGraph);
		void RemoveGraph(Graph* aGraph);

		void Imgui();

		bool ManageGraphs();

	private:

		std::vector<Graph*> myGraphs;
	};
}

#endif