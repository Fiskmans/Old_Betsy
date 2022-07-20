#include "engine/graph/Dependable.h"


namespace engine::graph
{
	void Dependable::MarkDirty()
	{
		if (!myIsDirty)
			for (Dependable* dependent : myDependents)
				dependent->MarkDirty();

		myIsDirty = true;
	}

	void
		Dependable::RemoveDependent(Dependable* aDependent)
	{
		decltype(myDependents)::iterator it = std::find(myDependents.begin(), myDependents.end(), aDependent);

		if (it != myDependents.end())
			myDependents.erase(it);
	}

	void Dependable::MarkRefreshed()
	{
		myIsDirty = false;
		for (Dependable* dep : myDependents)
		{
			dep->MarkDirty();
		}
	}

	void Dependable::AddDependent(Dependable* aDependent)
	{
		myDependents.push_back(aDependent);
		aDependent->MarkDirty();
	}
}