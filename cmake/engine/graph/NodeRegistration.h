#ifndef ENGINE_GRAPH_NODES_REGISTRATION_H
#define ENGINE_GRAPH_NODES_REGISTRATION_H

#include "tools/Singleton.h"

#include "engine/graph/nodes/RenderCopyNode.h"
#include "engine/graph/nodes/RenderMergeNode.h"
#include "engine/graph/nodes/TestNodes.h"
#include "engine/graph/nodes/ImageNode.h"
#include "engine/graph/nodes/MulNode.h"
#include "engine/graph/nodes/RenderGeometryNode.h"
#include "engine/graph/nodes/RenderDeferredNode.h"

#include "engine/graphics/Camera.h"
#include "engine/graphics/RenderScene.h"

#include "common/Macros.h"

namespace engine::graph
{

	class NodeRegistration : public tools::Singleton<NodeRegistration>
	{
	public:
		static void Register()
		{
			GetInstance();
		}

	private:

		template<typename MemberFunctionPointer>
		void RegisterMemberFunctionBasedNode(MemberFunctionPointer aFunctionPointer, const char* aName = "Function Node", const char* aOutPinName = "Out", std::vector<const char*> aPinNames = {})
		{
			myFreeNodes.push_back(new MemberCallbackBasedNode(aFunctionPointer, aName, aOutPinName, aPinNames));
		}

		NodeGroup<node::MulNode,
			int,
			float,
			size_t> UNIQUENAME;

		nodes::RenderCopyNode UNIQUENAME;
		nodes::RenderMergeNode UNIQUENAME;
		nodes::RenderGeometryNode UNIQUENAME;
		nodes::RenderDeferredNode UNIQUENAME;

		nodes::PrintNode UNIQUENAME;
		nodes::IntConstant UNIQUENAME;
		nodes::IntPassThrough UNIQUENAME;

		nodes::ImageNode UNIQUENAME;
		nodes::EngineTextureNode UNIQUENAME;

		ConversionNode<AssetHandle<DrawableTextureAsset>, AssetHandle<TextureAsset>> UNIQUENAME;

		std::vector<NodeBase*> myFreeNodes;
	};
}

#endif