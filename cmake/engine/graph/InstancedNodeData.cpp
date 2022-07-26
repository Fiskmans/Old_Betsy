#include "engine/graph/InstancedNodeData.h"

#include "engine/graph/NodeManager.h"

namespace engine::graph 
{
	namespace instanced_node_data_helpers
	{
		size_t RegisterNodeData(InstancedNodeDataBase* aData, size_t aSize, size_t aAlignement)
		{
			return NodeManager::GetInstance().AddData(aData, aSize, aAlignement);
		}
	}
}
