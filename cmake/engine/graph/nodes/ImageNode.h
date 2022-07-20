#ifndef ENGINE_GRAPH_NODES_IMAGE_NODE_H
#define ENGINE_GRAPH_NODES_IMAGE_NODE_H

#include "engine/graph/Node.h"
#include "engine/graph/NodePin.h"

#include "engine/assets/Asset.h"

#include <string>

namespace engine::graph::node
{
	class ImageNode : public NodeBase
	{
	public:

		void Activate(NodeInstanceId aId) override;

		ImVec2 ImguiSize(NodeInstanceId aId) override { return ImVec2(40, 40); }
		void Imgui(NodeInstanceId aId, float aScale, ImVec2 aTopLeft) override;

	private:
		InPin<std::string> myFilePath = "File path";

		OutPin<AssetHandle> myTexture = "Texture";
	};
}

#endif