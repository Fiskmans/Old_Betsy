#ifndef ENGINE_GRAPH_PIN_CONSTANT_H
#define ENGINE_GRAPH_PIN_CONSTANT_H

#include "engine/graph/PinValue.h"

namespace engine::graph
{
	class Dependable;

	template<class Type>
	class PinConstant : public PinValueBase
	{
	public:
		PinConstant(Dependable* aChild);
		void Unlink() override { delete this; }

	private:
		Type myStorage;
	};

	template<class Type>
	PinConstant<Type>::PinConstant(Dependable* aChild)
		: PinValueBase(myStorage)
		, myStorage{}
		
	{
		AddDependent(aChild);
		SetRefreshCallback([]() {});
		MarkRefreshed();
	}
}

#endif