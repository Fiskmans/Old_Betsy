#ifndef ENGINE_GRAPH_NODE_TEST_NODES_H
#define ENGINE_GRAPH_NODE_TEST_NODES_H

#include "engine/graph/Node.h"
#include "engine/graph/NodePin.h"

namespace engine::graph::node 
{
	class PrintNode : public NodeBase
	{
	public:
		void Activate(NodeInstanceId aId) override;

		ImVec2 ImguiSize(NodeInstanceId aId) override;
		void Imgui(NodeInstanceId aId, float aScale, ImVec2 aTopLeft) override;
	private:
		InPin<int> myIn = "Val";

		NodeData<int> myStored;
	};

	class IntConstant : public NodeBase
	{
	public:
		void Activate(NodeInstanceId aId) override;

		ImVec2 ImguiSize(NodeInstanceId aId) override;
		void Imgui(NodeInstanceId aId, float aScale, ImVec2 aTopLeft) override;
	private:
		OutPin<int> myOut = "Val";

		NodeData<int> myValue;
	};

	class IntPassThrough : public NodeBase
	{
	public:
		void Activate(NodeInstanceId aId) override { myOut.Write(aId, myIn.Get(aId)); }

	private:
		InPin<int> myIn = "In";
		OutPin<int> myOut = "Out";
	};
	
	class EngineTextureNode : public NodeBase 
	{
	public:
		void Activate(NodeInstanceId aId) override;

	private:
		InPin<int> mySelection = "Index";
		OutPin<std::string> myPath = "Tex Path";
	};
}

#endif