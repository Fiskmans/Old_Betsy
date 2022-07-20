#ifndef ENGINE_GRAPH_DEPENDABLE_H
#define ENGINE_GRAPH_DEPENDABLE_H

#include <vector>

namespace engine::graph
{
	class Dependable
	{
	public:
		virtual ~Dependable() = default;

		void AddDependent(Dependable* aDependent);
		void RemoveDependent(Dependable* aDependent);

		void MarkRefreshed();
		bool IsDirty() { return myIsDirty; }
	private:
		void MarkDirty();
		std::vector<Dependable*> myDependents;
		bool myIsDirty = true;
	};
}

#endif