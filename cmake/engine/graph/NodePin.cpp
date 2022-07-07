#include "engine/graph/NodePin.h"
#include "engine/graph/Node.h"
#include "engine/graph/NodeManager.h"

namespace engine::graph
{
	PinValueBase& InPinInstance::Fetch()
	{
		if (myTarget->IsDirty())
			myRefreshCallback();

		return *myTarget;
	}
	namespace node_pin_helpers
	{
		void RegisterInPin(PinBase* aPin)
		{
			NodeManager::GetInstance().AddInPin(aPin);
		}

		void RegisterOutPin(PinBase* aPin)
		{
			NodeManager::GetInstance().AddOutPin(aPin);
		}
	}
}

