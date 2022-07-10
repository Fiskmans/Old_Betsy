#ifndef ENGINE_GRAPH_GRAPH_H
#define ENGINE_GRAPH_GRAPH_H

#include "tools/Singleton.h"

#include "engine/graph/NodeManager.h"
#include "engine/graph/BuiltNode.h"
#include "engine/graph/NodeInstanceId.h"

#include "imgui/imgui.h"

#include <vector>
#include <memory>
#include <string>
#include <concepts>
#include <ranges>
#include <optional>

namespace engine::graph
{

	class Graph;

	class DrawablePinBlock
	{
	public:
		DrawablePinBlock(ImVec2 aPosition)
			: myPosition(aPosition)
		{

		}

		bool Imgui(const char* aName, Graph* aGraph, NodeInstanceId aId, float aScale, ImVec2 aPosition, const std::vector<PinBase*>& aInPins, const std::vector<PinBase*>& aOutPins);

		ImVec2 myPosition;
		bool myIsMoving = false;
	};

	class NodeInstance : private DrawablePinBlock
	{
	public:
		NodeInstance(BuiltNode& aType, ImVec2 aPosition);

		bool Imgui(Graph* aGraph, float aScale, ImVec2 aPosition);
	private:
		BuiltNode* myType;
		NodeInstanceId myId;
	};

	class GraphExportPinBlock : private DrawablePinBlock
	{
	public:
		GraphExportPinBlock(const std::vector<PinBase*>& aPins)
			: DrawablePinBlock(ImVec2(500,0))
			, myPins(aPins)
		{
		}

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

		bool Imgui(Graph* aGraph, float aScale, ImVec2 aPosition);

	private:
		std::vector<PinBase*> myPins;
	};

	class PinLink
	{
	public:
		PinLink(OutPinInstanceBase* aFrom, InPinInstance* aTo);
		~PinLink();

		void Imgui(Graph* aGraph,const std::unordered_map<PinInstanceBase*, ImVec2>& aLocations, float aScale, ImVec2 aPosition);

	private:
		OutPinInstanceBase* myFrom;
		InPinInstance*		myTo;
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

		void AddLink(OutPinInstanceBase* aFrom, InPinInstance* aTo);
		void RemoveLink(PinLink* aLink);

		void AddPinLocation(PinInstanceBase* aPin, ImVec2 aScreenPosition);

		const std::string& Name();
	private:

		PinLink* myLinkToRemove = nullptr;

		std::string myName;
		std::vector<std::unique_ptr<NodeInstance>> myNodes;
		std::vector<std::unique_ptr<PinLink>> myLinks;
		std::optional<GraphExportPinBlock> myExportBlock;
		std::optional<GraphImportPinBlock> myImportBlock;

		std::unordered_map<PinInstanceBase*, ImVec2> myPinPositions;
	};


	class GraphManager : public tools::Singleton<GraphManager>
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