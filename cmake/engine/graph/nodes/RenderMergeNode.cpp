#include "engine/graph/nodes/RenderMergeNode.h"

namespace engine::graph::node
{
	void RenderMergeNode::Activate(NodeInstanceId aId)
	{
		myOutTexture.Write(aId, myInTexture1.Get(aId));
	}
}