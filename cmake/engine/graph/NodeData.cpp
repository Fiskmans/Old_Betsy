#include "engine/graph/NodeData.h"

#include "engine/graph/NodeManager.h"

namespace engine::graph 
{
	namespace node_data_helpers
	{
		void RegisterData(NodeDataBase* aData)
		{
			NodeManager::GetInstance().AddData(aData);
		}
	}
}
