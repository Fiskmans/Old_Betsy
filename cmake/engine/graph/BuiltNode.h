#ifndef ENGINE_GRAPH_BUILT_NODE_H
#define ENGINE_GRAPH_BUILT_NODE_H

#include "engine/graph/NodePin.h"

#include <string>
#include <typeinfo>


namespace engine::graph
{
	class NodeBase;
	class NodeInstance;
	class InstancedNodeDataBase;

	class NodeInstanceDataBlobTemplate
	{
	public:
		size_t AddItem(size_t aSize, size_t aAligment);
		size_t Size() { return mySize; }

	private:
		size_t mySize = 0;
	};

	class BuiltNode;

	class NodeVarianceGroup
	{
	public:
		NodeVarianceGroup() = default;

	private:
		void AddNode(BuiltNode* aNode) { myVariances.push_back(aNode); };

		friend BuiltNode;
		std::vector<BuiltNode*> myVariances;
	};

	class BuiltNode
	{
	public:
		BuiltNode(NodeBase* aBaseNode, std::shared_ptr<NodeVarianceGroup>& aGroup);

		std::string Name();

		void AddInPin(PinBase* aInPin);
		void AddOutPin(PinBase* aOutPin);
		size_t AddData(InstancedNodeDataBase* aData, size_t aSize, size_t aAligment);

		void Imgui();

		ImVec2 ImguiSize();
		void Imgui(float aScale, ImVec2 aTopLeft);
		
		std::vector<PinBase*>& InPins() { return myInPins; }
		std::vector<PinBase*>& OutPins() { return myOutPins; }

		void Construct(NodeInstance* aInstance);
		void Destruct(NodeInstance* aInstance);
	private:
		friend NodeInstance;

		NodeBase* myBaseNode;
		std::shared_ptr<NodeVarianceGroup> myGroup;

		NodeInstanceDataBlobTemplate myMemoryLayoutTemplate;
		std::vector<PinBase*> myInPins;
		std::vector<PinBase*> myOutPins;
		std::vector<InstancedNodeDataBase*> myData;
	};
}

#endif