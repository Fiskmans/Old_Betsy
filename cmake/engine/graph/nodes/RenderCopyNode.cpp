#include "engine/graph/nodes/RenderCopyNode.h"


namespace engine::graph::node 
{
	void RenderCopyNode::Activate(NodeInstanceId aId)
	{
		myOutTexture.Write(aId, myInTexture.Get(aId));

		myIntPin.Write(aId, 5);
		myFloatPin.Write(aId, 3.f);
		myRandom.Write(aId, this);
	}
}