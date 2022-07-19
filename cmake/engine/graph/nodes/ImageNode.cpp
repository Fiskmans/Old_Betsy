#include "engine/graph/nodes/ImageNode.h"

#include "engine/assets/AssetManager.h"

namespace engine::graph::node
{
	void ImageNode::Activate(NodeInstanceId aId)
	{
		myTexture.Write(aId, AssetManager::GetInstance().GetTexture(myFilePath.Get(aId)));
	}

}
