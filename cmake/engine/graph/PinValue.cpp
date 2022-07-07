#include "engine/graph/PinValue.h"

namespace engine::graph
{
	void
	PinValueBase::MarkDirty()
	{
		if (!myIsDirty)
			for (PinValueBase* dependent : myDependents)
				dependent->MarkDirty();

		myIsDirty = true;
	}

	void
	PinValueBase::RemoveDependent(PinValueBase* aDependent)
	{
		decltype(myDependents)::iterator it = std::find(myDependents.begin(), myDependents.end(), aDependent);

		if (it != myDependents.end())
			myDependents.erase(it);
	}

}