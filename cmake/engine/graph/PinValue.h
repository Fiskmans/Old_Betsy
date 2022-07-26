#ifndef ENGINE_GRAPH_PIN_VALUE_H
#define ENGINE_GRAPH_PIN_VALUE_H

#include "engine/graph/Dependable.h"

#include "imgui/imgui.h"

#include <functional>

namespace engine::graph
{
	class PinValueBase : public Dependable
	{
	public:
		virtual ~PinValueBase() = default;
		virtual bool IsConstant() { return false;  }

		template <class T>
		T& As() { return *reinterpret_cast<T*>(GetRaw()); }

		void Load()
		{
			if (IsDirty())
				myCallback();
		}

		virtual void Unlink() {}

		virtual ImVec2 ImGuiSize() { return ImVec2(0, 0); };
		virtual void ImGui(float aScale, ImVec2 aLocation) {};
		void SetRefreshCallback(std::function<void()> aCallback) { myCallback = aCallback; }
	protected:
		virtual void* GetRaw() = 0;

	private:
		std::function<void()> myCallback;
	};

	template <class Type>
	class PinValue : public PinValueBase
	{
	public:
		PinValue() = default;

		void operator=(const Type& aValue) 
		{ 
			myStorage = aValue;
			MarkRefreshed();
		}

		Type& Get() { return myStorage; }

	private:
		void* GetRaw() override { return &myStorage; }

		Type myStorage;
	};
}

#endif