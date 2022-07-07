#ifndef ENGINE_GRAPH_NODE_PIN_H
#define ENGINE_GRAPH_NODE_PIN_H

#include "engine/graph/NodeInstanceId.h"
#include "engine/graph/NodeInstanceDataCollection.h"
#include "engine/graph/PinValue.h"

#include "imgui/imgui.h"

#include <typeinfo>
#include <functional>

namespace engine::graph
{
	class PinBase
	{
	public:
		virtual ~PinBase() = default;
		virtual const char*		Name() = 0;
		virtual const std::type_info& Type() = 0;

		void Draw(ImVec2 aLocation, bool aHovered);
		//static void Setup();
	};

	class OutPinInstanceBase
	{
	public:
		virtual ~OutPinInstanceBase() = default;
		virtual PinValueBase& GetStorage() = 0;
	};

	template<class T>
	class OutPinInstance : public OutPinInstanceBase
	{
	public:
		virtual ~OutPinInstance() = default;

		void operator=(const T& aValue) { myStorage = aValue; }
		PinValueBase& GetStorage() override { return myStorage; }

	private:

		PinValue<T> myStorage;
	};

	class InPinInstance
	{
	public:
		PinValueBase& Fetch();

	private:
		PinValueBase* myTarget;
		std::function<void()> myRefreshCallback;
	};

	namespace node_pin_helpers
	{
		void RegisterInPin(PinBase* aPin);
		void RegisterOutPin(PinBase* aPin);
	}

	template<class PinType> 
	class InPin : public PinBase
	{
	public:
		InPin(const char* aPinName) : myPinName(aPinName) 
		{ 
			node_pin_helpers::RegisterInPin(this); 
		}
		
		const char* Name() override { return myPinName.c_str(); }
		const std::type_info& Type() override { return typeid(PinType); }

		PinType& Get(NodeInstanceId aId) { return myInstances.Get(aId).Fetch().As<PinType>(); }

		void AddInstance(NodeInstanceId aId) { myInstances.AddInstance(aId); }
		void RemoveInstance(NodeInstanceId aId) { myInstances.RemoveInstance(aId); };

	private:
		std::string myPinName;
		NodeInstandeDataCollection<InPinInstance> myInstances;
	};

	template<class PinType>
	class OutPin : public PinBase
	{
	public:
		OutPin(const char* aPinName) : myPinName(aPinName) 
		{ 
			node_pin_helpers::RegisterOutPin(this); 
		}
		
		const char* Name() override { return myPinName.c_str(); }
		const std::type_info& Type() override { return typeid(PinType); }

		void Write(NodeInstanceId aId, const PinType& aValue) { return myInstances.Get(aId) = aValue; }

		void AddInstance(NodeInstanceId aId) { myInstances.AddInstance(aId); }
		void RemoveInstance(NodeInstanceId aId) { myInstances.RemoveInstance(aId); };

	private:
		std::string											myPinName;
		NodeInstandeDataCollection<OutPinInstance<PinType>> myInstances;
	};
}

#endif