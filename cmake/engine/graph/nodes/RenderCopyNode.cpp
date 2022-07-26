#include "engine/graph/nodes/RenderCopyNode.h"


namespace engine::graph::node 
{
	void RenderCopyNode::Activate()
	{
		myOutTexture = myInTexture;

		myIntPin = 5;
		myFloatPin = 3.f;
		myRandom = this;
	}
}