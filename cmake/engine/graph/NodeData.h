#ifndef ENGINE_GRAPH_NODE_DATA_H
#define ENGINE_GRAPH_NODE_DATA_H

#include "engine/graph/NodeInstanceDataCollection.h"

namespace engine::graph
{
	class NodeDataBase;
	namespace node_data_helpers
	{
		void RegisterData(NodeDataBase* aData);
	}

	class NodeDataBase
	{
	public:
		NodeDataBase() { node_data_helpers::RegisterData(this); }
		virtual ~NodeDataBase() = default;

		virtual void AddInsitance(NodeInstanceId aId) = 0;
		virtual void RemoveInstance(NodeInstanceId aId) = 0;
	};

	template<class Type>
	class NodeData : NodeDataBase
	{
	public:
		Type& Get(NodeInstanceId aId) { return myCollection.Get(aId); }

		void AddInsitance(NodeInstanceId aId) override { myCollection.AddInstance(aId); }
		void RemoveInstance(NodeInstanceId aId) override { myCollection.RemoveInstance(aId); }

	private:
		NodeInstandeDataCollection<Type> myCollection;
	};

}

#endif