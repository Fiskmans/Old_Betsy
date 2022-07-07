#ifndef ENGINE_GRAPH_PIN_VALUE_H
#define ENGINE_GRAPH_PIN_VALUE_H

#include <vector>

namespace engine::graph
{
	class PinValueBase
	{
	public:
		virtual ~PinValueBase() = default;

		template <class T>
		T& As() { return *reinterpret_cast<T*>(myValue); }

		bool IsDirty() { return myIsDirty; }
		void MarkDirty();
		void MarkRefreshed() { myIsDirty = false; }
		void AddDependent(PinValueBase* aDependent) { myDependents.push_back(aDependent); }
		void RemoveDependent(PinValueBase* aDependent);

	protected:
		template <class T>
		PinValueBase(T & aValue)
			: myValue(&aValue)
		{
		}

	private:
		void* myValue = nullptr;
		std::vector<PinValueBase*> myDependents;
		bool myIsDirty = true;
	};

	template <class Type>
	class PinValue : public PinValueBase
	{
	public:
		PinValue()
			: PinValueBase(myStorage)
		{
		}

		void operator=(const Type& aValue) 
		{ 
			myStorage = aValue;
			MarkRefreshed();
		}

		Type& Get() { return myStorage; }

	private:
		Type myStorage;
	};
}

#endif