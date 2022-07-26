#ifndef ENGINE_GRAPH_NODE_TEST_NODES_H
#define ENGINE_GRAPH_NODE_TEST_NODES_H

#include "engine/graph/Node.h"
#include "engine/graph/NodePin.h"

namespace engine::graph::node 
{
	class PrintNode : public NodeBase
	{
	public:
		void Activate() override;

		ImVec2 ImguiSize() override;
		void Imgui(float aScale, ImVec2 aTopLeft) override;
	private:
		InPin<int> myIn = PinInformation("Val");

		InstancedNodeData<int> myStored = std::string("Storage");
	};

	class IntConstant : public NodeBase
	{
	public:
		void Activate() override;

		ImVec2 ImguiSize() override;
		void Imgui(float aScale, ImVec2 aTopLeft) override;
	private:
		OutPin<int> myOut = PinInformation("Val");

		InstancedNodeData<int> myValue = std::string("Storage");
	};

	class IntPassThrough : public NodeBase
	{
	public:
		void Activate() override { myOut = myIn; }

	private:
		InPin<int> myIn = PinInformation("In");
		OutPin<int> myOut = PinInformation("Out");
	};

	class EngineTextureNode : public NodeBase
	{
	public:
		void Activate() override;

	private:
		InPin<int> mySelection = PinInformation("Index");
		OutPin<std::string> myPath = PinInformation("Tex Path");
	};
}

#endif