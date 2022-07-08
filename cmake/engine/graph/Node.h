#ifndef ENGINE_GRAPH_PIN_H
#define ENGINE_GRAPH_PIN_H

#include "engine/graph/NodeManager.h"

#include "tools/Singleton.h"


namespace engine::graph
{
	template<class Derived>
	class NodeBase : public tools::Singleton<NodeBase<Derived>>
	{
	public:
		NodeBase()
		{
			NodeManager::GetInstance().BeginNode(typeid(*this).name());
		}
		~NodeBase() = default;


		static void Register()
		{
			static_cast<void>(Derived::GetInstance());
		}
		
		virtual void Imgui(NodeInstanceId aId) {};
	};
}

#endif