#pragma once
#include "CommonUtilities/Singleton.hpp"
#include "me"
#define CU CommonUtilities

template<typename T>
class FactoryComplex : public CU::Singleton<FactoryComplex<T>>
{
	friend class CU::Singleton<FactoryComplex<T>>;
public:
	T* Create(LoadData* aData);
private:
};

template<typename T>
inline T * FactoryComplex<T>::Create(LoadData * aData)
{
	static_assert(std::is_base_of<Entity, T>::value, "Object is not an Entity");

	T* ptr = new T();
	ptr->Load(aData);
	return ptr;
}
