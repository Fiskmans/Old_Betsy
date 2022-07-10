#ifndef ENGINE_GRAPH_NODE_INSTANCE_ID_H
#define ENGINE_GRAPH_NODE_INSTANCE_ID_H

namespace engine::graph
{
	using NodeInstanceId = size_t;
	const NodeInstanceId InvalidNodeInstanceId = 0;

	inline NodeInstanceId GetNextNodeInstanceId()
	{
		static NodeInstanceId counter = InvalidNodeInstanceId;
		return ++counter;
	}
}

#endif