#ifndef ENGINE_GRAPH_NODES_RENDER_GEOMETRY_NODE_H
#define ENGINE_GRAPH_NODES_RENDER_GEOMETRY_NODE_H

#include "engine/graph/Node.h"
#include "engine/graph/NodePin.h"

#include "engine/graphics/GBuffer.h"
#include "engine/graphics/Camera.h"
#include "engine/graphics/Texture.h"

#include "engine/assets/Asset.h"
#include "engine/assets/ModelInstance.h"

#include "tools/MathVector.h"

#include <d3d11.h>

namespace engine::graph::nodes
{
	class RenderGeometryNode : engine::graph::NodeBase
	{
	public:
		void Activate() override;

		ImVec2 ImguiSize() override { return ImVec2(80, 160); }
		void Imgui(float aScale, ImVec2 aTopLeft) override;

	private:
		InPin<Camera*> myInCamera = PinInformation("Camera");
		InPin<tools::V2ui> myInResolution = PinInformation("Resolution");
		InPin<AssetHandle<DepthTextureAsset>> myInDepth = PinInformation("Depth");

		InstancedNodeData<AssetHandle<DepthTextureAsset>> myStoredDepth = std::string("StoredDepth");

		OutPin<AssetHandle<DepthTextureAsset>> myOutDepth = PinInformation("Depth");
		OutPin<AssetHandle<GBufferAsset>> myOutGeometry = PinInformation("Geometry");
	};
}

#endif