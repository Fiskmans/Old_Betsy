#ifndef ENGINE_GRAPH_NODES_IMAGE_NODE_H
#define ENGINE_GRAPH_NODES_IMAGE_NODE_H

#include "engine/graph/Node.h"
#include "engine/graph/NodePin.h"

#include "engine/assets/Asset.h"

#include <string>

namespace engine::graph::nodes
{
	class ImageNode : public NodeBase
	{
	public:

		void Activate() override;

		ImVec2 ImguiSize() override { return ImVec2(40, 40); }
		void Imgui(float aScale, ImVec2 aTopLeft) override;

	private:
		InPin<std::string> myFilePath = PinInformation("File path");

		OutPin<AssetHandle> myTexture = PinInformation("Texture");
	};
}

#endif