#ifndef ENGINE_GRAPH_NODE_DATA_H
#define ENGINE_GRAPH_NODE_DATA_H

#include <cstddef>
#include <utility>
#include <string>

namespace engine::graph
{
	class InstancedNodeDataBase;
	extern std::byte* globalCurrentInstanceContext;

	namespace instanced_node_data_helpers
	{
		size_t RegisterNodeData(InstancedNodeDataBase* aData, size_t aSize, size_t aAlignement);
	}

	class InstancedNodeDataBase
	{
	public:
		virtual void Construct() = 0;
		virtual void Destruct() = 0;

	private:
		friend class BuiltNode;
		virtual size_t Size() = 0;
		virtual size_t Offset() = 0;
		virtual const char* Name() = 0;
	};

	template<class T>
	class InstancedNodeData : public InstancedNodeDataBase
	{
	public:
		InstancedNodeData(const std::string& aName)
			: myOffset(instanced_node_data_helpers::RegisterNodeData(this, sizeof(T), alignof(T)))
			, myName(std::string("[") + typeid(T).name() + "] " + aName)
		{
		}

		template<class Other>
		auto operator=(const Other&& aOther) { return Get() = std::forward<Other>(aOther); }

		T& operator=(const T& aValue) { return Get() = aValue; }

		T* GetPointer() { return reinterpret_cast<T*>(globalCurrentInstanceContext + myOffset); }
		T& Get() { return *GetPointer(); }
		operator T& () { return Get(); }

		void Construct() override { std::construct_at<T>(GetPointer()); }
		void Destruct() override { std::destroy_at<T>(GetPointer()); }

	private:

		size_t Size() override { return sizeof(T); };
		size_t Offset() override { return myOffset; };
		const char* Name() override { return myName.c_str(); }

		size_t myOffset;
		std::string myName;
	};
}

#endif