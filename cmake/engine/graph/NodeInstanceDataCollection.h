#ifndef ENGINE_GRAPH_NODE_INSTANCE_DATA_COLLECTION_H
#define ENGINE_GRAPH_NODE_INSTANCE_DATA_COLLECTION_H

#include "engine/graph/NodeInstanceId.h"

#include <memory>
#include <vector>

namespace engine::graph
{
	template<class T>
	struct NodeInstanceData
	{
		NodeInstanceId myId;
		T myData;

		auto operator <=>(NodeInstanceData& aOther) { return myId <=> aOther.myId; }
		auto operator <=>(NodeInstanceId aId) { return myId <=> aId; }
	};

	template<class T>
	class NodeInstandeDataCollection
	{
	private: 
		T& Get(NodeInstanceId aId);
		void AddInstance(NodeInstanceId aId);
		void RemoveInstance(NodeInstanceId aId);
	private:

		typename std::vector<std::unique_ptr<NodeInstanceData<T>>>::iterator IndexOf(NodeInstanceId aId);
		std::vector<std::unique_ptr<NodeInstanceData<T>>> myInstances;
	};

	//////////////////////////////////////////////////////////////////////////
	
	template<class T>
	inline T& NodeInstandeDataCollection<T>::Get(NodeInstanceId aId)
	{
		return IndexOf(aId)->myData;
	}

	template<class T>
	inline void NodeInstandeDataCollection<T>::AddInstance(NodeInstanceId aId)
	{
		myInstances.emplace(IndexOf(aId), std::make_unique<NodeInstanceData<T>>());
	}

	template<class T>
	inline void NodeInstandeDataCollection<T>::RemoveInstance(NodeInstanceId aId)
	{
		myInstances.erase(IndexOf(aId));
	}

	template<class T>
	inline typename std::vector<std::unique_ptr<NodeInstanceData<T>>>::iterator NodeInstandeDataCollection<T>::IndexOf(NodeInstanceId aId)
	{
		return std::lower_bound(myInstances.begin(), myInstances.end(), aId);
	}
}

#endif