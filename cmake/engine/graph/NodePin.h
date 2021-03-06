#ifndef ENGINE_GRAPH_NODE_PIN_H
#define ENGINE_GRAPH_NODE_PIN_H

#include "engine/graph/NodeInstanceId.h"
#include "engine/graph/NodeInstanceDataCollection.h"
#include "engine/graph/PinValue.h"
#include "engine/graph/Dependable.h"
#include "engine/graph/PinConstant.h"

#include "imgui/imgui.h"

#include <typeinfo>
#include <functional>

namespace engine::graph
{
	class Graph;
	class OutPinInstanceBase;
	class InPinInstanceBase;

	class PinBase
	{
	public:
		virtual ~PinBase() = default;
		virtual const char*		Name() const = 0;
		virtual const std::type_info& Type() const = 0;
		virtual bool IsInPin() const = 0;

		virtual void AddInstance(NodeInstanceId aId) {}
		virtual void RemoveInstance(NodeInstanceId aId) {}

		virtual OutPinInstanceBase* GetOutPinInstance(NodeInstanceId aId) { return nullptr; }
		virtual InPinInstanceBase* GetInPinInstance(NodeInstanceId aId) { return nullptr; }

		virtual PinValueBase* GetOutStorage(NodeInstanceId aId) { return nullptr; }

		bool CanConnectTo(const PinBase& aOther) const;

		ImVec2 ImGuiSize(NodeInstanceId aId);
		bool ImGui(Graph* aGraph, float aScale, ImVec2 aLocation, NodeInstanceId aId, ImVec2& aOutAttachPoint);
		//static void Setup();

		static void UpdateImGui();
	private:
		bool myIsHovered = false;

		static const std::type_info* ourHoveredType;
		static const std::type_info* ourNextHoveredType;
		static float ourHoverIntensity;
		static bool ourHoverIn;
		static ImVec2 ourHoverTarget;
		static bool ourIsHoveringTarget;
		static bool ourNextIsHoveringTarget;
	};

	class PinInstanceBase
	{
	public:
		virtual ~PinInstanceBase() = default;
	};

	class OutPinInstanceBase : public PinInstanceBase
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

	class InPinInstanceBase : 
		public PinInstanceBase,
		public Dependable
	{
	public:
		PinValueBase& Fetch();

		void LinkTo(OutPinInstanceBase* aPin);
		void UnlinkFrom(OutPinInstanceBase* aPin);

		ImVec2 CustomImguiSize();
		void CustomImgui(float aScale, ImVec2 aLocation);

	protected:
		virtual void AttachConstant() = 0;

		friend Graph;

		template<class Type>
		friend class InPin;

		PinValueBase* myTarget;
	};

	template<class Type>
	class InPinInstance : public InPinInstanceBase
	{
	public:
		InPinInstance() 
		{
			AttachConstant();
		}

	private:
		void AttachConstant() override
		{
			myTarget = new PinConstant<Type>(this);
			myTarget->AddDependent(this);
		}
	};

	namespace node_pin_helpers
	{
		void RegisterInPin(PinBase* aPin);
		void RegisterOutPin(PinBase* aPin);
	}

	template<class PinType>
	class CustomInPin : public PinBase
	{
	public:
		CustomInPin(const char* aPinName) : myPinName(aPinName)
		{
		}

		const char* Name() const override { return myPinName.c_str(); }
		const std::type_info& Type() const override { return typeid(PinType); }
		bool IsInPin() const override { return true; }

		InPinInstanceBase* GetInPinInstance(NodeInstanceId aId) override { return &myValue; }

		PinType& Get() { return myValue.Fetch().template As<PinType>(); }
	private:
		std::string myPinName;
		InPinInstance<PinType> myValue;
	};

	template<class PinType>
	class CustomOutPin : public PinBase
	{
	public:
		CustomOutPin(const char* aPinName) : myPinName(aPinName)
		{
		}

		const char* Name() const override { return myPinName.c_str(); }
		const std::type_info& Type() const override { return typeid(PinType); }
		bool IsInPin() const override { return false; }

		PinValueBase* GetOutStorage(NodeInstanceId aId) override { return &myValue.GetStorage(); }
		OutPinInstanceBase* GetOutPinInstance(NodeInstanceId /*aId*/) override { return &myValue; }
		
		void Write(const PinType& aValue) 
		{
			myValue = aValue;
		}

		PinType& Get() { return myValue.GetStorage().template As<PinType>(); }
	private:
		std::string myPinName;
		OutPinInstance<PinType> myValue;
	};

	template<class PinType> 
	class InPin : public PinBase
	{
	public:
		InPin(const char* aPinName) : myPinName(aPinName) 
		{ 
				node_pin_helpers::RegisterInPin(this); 
		}
		
		const char* Name() const override { return myPinName.c_str(); }
		const std::type_info& Type() const override { return typeid(PinType); }
		bool IsInPin() const override { return true; }

		void AddInstance(NodeInstanceId aId) override 
		{ 
			myInstances.AddInstance(aId); 
		}

		void RemoveInstance(NodeInstanceId aId) override { myInstances.RemoveInstance(aId); };

		InPinInstanceBase* GetInPinInstance(NodeInstanceId aId) override { return &myInstances.Get(aId); }

		PinType& Get(NodeInstanceId aId) { return myInstances.Get(aId).Fetch().template As<PinType>(); }

	private:
		std::string myPinName;
		NodeInstandeDataCollection<InPinInstance<PinType>> myInstances;
	};

	template<class PinType>
	class OutPin : public PinBase
	{
	public:
		OutPin(const char* aPinName) : myPinName(aPinName)
		{
				node_pin_helpers::RegisterOutPin(this); 
		}
		
		const char* Name() const override { return myPinName.c_str(); }
		const std::type_info& Type() const override { return typeid(PinType); }
		bool IsInPin() const override { return false; }
 
		PinValueBase* GetOutStorage(NodeInstanceId aId) override { return &myInstances.Get(aId).GetStorage(); }

		void AddInstance(NodeInstanceId aId) override { myInstances.AddInstance(aId); }
		void RemoveInstance(NodeInstanceId aId) override { myInstances.RemoveInstance(aId); };

		OutPinInstanceBase* GetOutPinInstance(NodeInstanceId aId) override { return &myInstances.Get(aId); }

		void Write(NodeInstanceId aId, const PinType& aValue) { return myInstances.Get(aId) = aValue; }

	private:
		std::string											myPinName;
		NodeInstandeDataCollection<OutPinInstance<PinType>> myInstances;
	};
}

#endif