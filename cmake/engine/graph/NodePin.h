#ifndef ENGINE_GRAPH_NODES_PIN_H
#define ENGINE_GRAPH_NODES_PIN_H

#include "engine/graph/NodeInstanceId.h"

#include "engine/graph/PinValue.h"
#include "engine/graph/Dependable.h"
#include "engine/graph/PinConstant.h"
#include "engine/graph/InstancedNodeData.h"

#include "imgui/imgui.h"

#include <typeinfo>
#include <functional>

namespace engine::graph
{
	class Graph;
	class OutPinInstanceBase;
	class InPinInstanceBase;

	class PinInformation
	{
	public:
		PinInformation(const std::string& aName)
			: myName(aName)
		{
		};

		const char* Name() const { return myName.c_str(); }
	private:
		std::string myName;
	};

	class PinBase
	{
	public:
		explicit PinBase(const PinInformation& aInformation) : myInformation(aInformation) {}
		virtual ~PinBase() = default;

		void UpdateInformation(const PinInformation& aInformation) { myInformation = aInformation; }
		const char* Name() const { return myInformation.Name(); };
		virtual const std::type_info& Type() const = 0;
		virtual bool IsInPin() const = 0;

		virtual OutPinInstanceBase* GetOutPinInstance() { return nullptr; }
		virtual InPinInstanceBase* GetInPinInstance() { return nullptr; }

		virtual PinValueBase* GetOutStorage() { return nullptr; }

		bool CanConnectTo(const PinBase& aOther) const;

		ImVec2 ImGuiSize();
		bool ImGui(Graph* aGraph, float aScale, ImVec2 aLocation, ImVec2& aOutAttachPoint);
		//static void Setup();

		static void UpdateImGui();
	private:
		PinInformation myInformation;
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
		virtual const PinValueBase& GetStorage() const = 0;
	};

	template<class T>
	class OutPinInstance : public OutPinInstanceBase
	{
	public:
		virtual ~OutPinInstance() = default;

		void operator=(const T& aValue) { myStorage = aValue; }
		PinValueBase& GetStorage() override { return myStorage; }
		const PinValueBase& GetStorage() const override { return myStorage; }

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
		CustomInPin(PinInformation aPinInformation) : PinBase(aPinInformation)
		{
		}

		const std::type_info& Type() const override { return typeid(PinType); }
		bool IsInPin() const override { return true; }

		InPinInstanceBase* GetInPinInstance() override { return &myValue; }

		PinType& Get() { return myValue.Fetch().template As<PinType>(); }
	private:
		InPinInstance<PinType> myValue;
	};

	template<class PinType>
	class CustomOutPin : public PinBase
	{
	public:
		CustomOutPin(PinInformation aPinInformation) : PinBase(aPinInformation)
		{
		}

		const std::type_info& Type() const override { return typeid(PinType); }
		bool IsInPin() const override { return false; }

		PinValueBase* GetOutStorage() override { return &myValue.GetStorage(); }
		OutPinInstanceBase* GetOutPinInstance() override { return &myValue; }

		void Write(const PinType& aValue) { myValue = aValue; }
		void operator=(const PinType& aValue) { Write(aValue); }

		PinType& Get() { return myValue.GetStorage().template As<PinType>(); }
		const PinType& Get() const { return myValue.GetStorage().template As<PinType>(); }
	private:
		OutPinInstance<PinType> myValue;
	};

	template<class PinType>
	class InPin : public PinBase
	{
	public:
		InPin(PinInformation aPinInformation = PinInformation("in"))
			: PinBase(aPinInformation)
			, myInstances(std::string("InPin-") + Name())
		{
			node_pin_helpers::RegisterInPin(this);
		}

		const std::type_info& Type() const override { return typeid(PinType); }
		bool IsInPin() const override { return true; }

		InPinInstanceBase* GetInPinInstance() override { return &myInstances.Get(); }

		PinType& Get() { return myInstances.Get().Fetch().template As<PinType>(); }
		operator PinType& () { return Get(); }

	private:
		InstancedNodeData<InPinInstance<PinType>> myInstances;
	};

	template<class PinType>
	class OutPin : public PinBase
	{
	public:
		OutPin(PinInformation aPinInformation = PinInformation("Out"))
			: PinBase(aPinInformation)
			, myInstances(std::string("OutPin-") + Name())
		{
			node_pin_helpers::RegisterOutPin(this); 
		}
		
		const std::type_info& Type() const override { return typeid(PinType); }
		bool IsInPin() const override { return false; }
 
		PinValueBase* GetOutStorage() override { return &myInstances.Get().GetStorage(); }

		OutPinInstanceBase* GetOutPinInstance() override { return &myInstances.Get(); }

		void Write(const PinType& aValue) { return myInstances.Get() = aValue; }
		void operator=(const PinType& aValue) { Write(aValue); }

	private:
		InstancedNodeData<OutPinInstance<PinType>> myInstances;
	};
}

#endif