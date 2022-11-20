#ifndef ENGINE_GRAPH_PIN_H
#define ENGINE_GRAPH_PIN_H

#include "engine/graph/NodeManager.h"
#include "engine/graph/NodePin.h"

#include "tools/Functors.h"

#include <tuple>

namespace engine::graph
{
	class NodeBase
	{
	public:
		NodeBase()
		{
			NodeManager::GetInstance().BeginNode(this);
		}
		virtual ~NodeBase() = default;
		
		virtual void Activate() = 0;
		virtual std::string Name()
		{
			return NodeManager::PrettyfyName(typeid(*this).name()).c_str();
		}

		virtual ImVec2 ImguiSize() { return ImVec2(0, 0); }
		virtual void Imgui(float aScale, ImVec2 aTopLeft) {};
	};

	template<typename ReturnType, class Type, typename... Args>
	class MemberCallbackBasedNode : public NodeBase
	{
	public:
		using MemberFunctionPointer = ReturnType(Type::*)(Args...) const;

		template<size_t Index>
		void ApplyPinName(const std::vector<const char*>& aNames)
		{
			std::get<Index>(myInPins).UpdateInformation(PinInformation(aNames.size() > Index ? aNames[Index] : ("In " + std::to_string(Index))));
		}

		template<size_t... IndexSequence>
		void ApplyPinNames(const std::vector<const char*>& aPinNames, std::index_sequence<IndexSequence...> DummyArg)
		{
			static_cast<void>(DummyArg);

			int _[] = {0, (ApplyPinName<IndexSequence>(aPinNames), 0)...};
			static_cast<void>(_);
		}

		MemberCallbackBasedNode(MemberFunctionPointer aCallback, const char* aName, const char* aInPinName, const std::vector<const char*>& aPinNames)
			: myCallback(aCallback)
			, myName(aName)
		{
			myOutPin.UpdateInformation(PinInformation(aInPinName));
			ApplyPinNames(aPinNames, std::make_index_sequence<std::tuple_size_v<decltype(myInPins)>>{});
		}

		std::string Name() override
		{
			return myName;
		}

		void Activate() override
		{
			myOutPin = std::apply(myCallback, myInPins);
		}

	private:
		
		MemberFunctionPointer myCallback;
		std::string myName;


		OutPin<ReturnType> myOutPin;
		std::tuple<InPin<Type*>, InPin<std::remove_const_t<std::remove_reference_t<Args>>>...> myInPins;
	};


	template<typename From, typename To>
	class ConversionNode : public NodeBase
	{
	public:
		void Activate() { myTo = static_cast<To>(myFrom.Get()); }

	private:
		InPin<From> myFrom = PinInformation("In");
		OutPin<To> myTo = PinInformation("Out");
	};

	class NodeBeginGroupType
	{
	public:
		NodeBeginGroupType()
		{
			NodeManager::GetInstance().BeginGroup();
		}
	};

	class NodeEndGroupType
	{
	public:
		NodeEndGroupType()
		{
			NodeManager::GetInstance().BeginGroup();
		}
	};

	template<template<class T> class NodeType, typename... AcceptableTypes>
	class NodeGroup
	{
		NodeBeginGroupType myBegin;

		std::tuple<NodeType<AcceptableTypes>...> myNodes;

		NodeEndGroupType myEnd;
	};
}

#endif