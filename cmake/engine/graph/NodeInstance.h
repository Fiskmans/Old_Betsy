#ifndef ENGINE_GRAPH_NODES_INSTANCE_H
#define ENGINE_GRAPH_NODES_INSTANCE_H

#include "engine/graph/DrawablePinBlock.h"
#include "engine/graph/BuiltNode.h"	

#include "imgui/imgui.h"

#include <vector>
#include <stack>
#include <memory>

namespace engine::graph
{
	class BuiltNode;
	class PinLink;

	class NodeInstanceScopedContext
	{
	public:
		NodeInstanceScopedContext(std::byte* aContext);
		~NodeInstanceScopedContext();

	private:
		static std::stack<std::byte*> ourContextStack;
	};

	class NodeInstance : public DrawablePinBlock
	{
	public:
		NodeInstance(BuiltNode& aType, ImVec2 aPosition);
		virtual ~NodeInstance();

		bool Imgui(Graph* aGraph, float aScale, ImVec2 aPosition);

		void Activate();

		ImVec2 CustomImguiSize() override;
		void CustomImgui(float aScale, ImVec2 aTopLeft) override;

		void RemoveAllRelatedLinks(std::vector<std::unique_ptr<PinLink>>& aLinks);

	private:
		friend BuiltNode;

		BuiltNode& myType;
		std::vector<std::byte> myMemory;
		NodeInstanceId myId;
	};
}

#endif