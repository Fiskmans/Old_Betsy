#ifndef ENGINE_GRAPH_BUILT_NODE_H
#define ENGINE_GRAPH_BUILT_NODE_H

#include "engine/graph/NodePin.h"

#include <string>

namespace engine::graph
{
	class BuiltNode
	{
	public:
		BuiltNode(const std::string& aName);

		const std::string& Name() { return myName; }

		void AddInPin(PinBase* aInPin);
		void AddOutPin(PinBase* aOutPin);

		void Imgui();

		std::vector<PinBase*>& InPins() { return myInPins; }
		std::vector<PinBase*>& OutPins() { return myOutPins; }

	private:

		std::string myName;

		std::vector<PinBase*> myInPins;
		std::vector<PinBase*> myOutPins;
	};
}

#endif