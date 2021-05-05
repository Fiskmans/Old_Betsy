#pragma once
#include "Singleton.hpp"
#include "Entity.h"
//#include <CommonUtilities/Singleton.hpp>
#define CU CommonUtilities

template<typename T>
class FactoryComplex : public CommonUtilities::Singleton<FactoryComplex<T>>
{
	friend class CommonUtilities::Singleton<FactoryComplex<T>>;
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
