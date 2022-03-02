
#ifndef TOOLS_FISK_JSON_H
#define TOOLS_FISK_JSON_H

#include <string>
#include <unordered_map>
#include <optional>
#include <variant>
#include <vector>

#include "DereferencingIteratorWrapper.h"

namespace Tools::FiskJSON
{
	class Key_Unavailable : public std::exception
	{
	public:
		Key_Unavailable(const std::string& aMessage) : std::exception(aMessage.c_str()) {}
	};

	class Invalid_JSON : public std::exception
	{
	public:
		Invalid_JSON(const std::string& aMessage) : std::exception(aMessage.c_str()) {}
	};

	class Invalid_Object : public std::exception
	{
	public:
		Invalid_Object(const std::string& aMessage) : std::exception(("This object has been severely invalidated: " + aMessage).c_str()) {}
	};

	class Invalid_Get: public std::exception
	{
	public:
		Invalid_Get(const std::string& aMessage) : std::exception(aMessage.c_str()) {}
	};

	void replaceAll(std::string& str, const std::string& from, const std::string& to);
	
	class Object
	{
		friend class ArrayWrapper;
		friend class ConstArrayWrapper;
	public:
		Object& Parse(const std::string& aDocument);
		Object& Parse(const char* aBegin, const char* aEnd);

		~Object();
		Object() = default;
		Object(const Object& aOther) = delete;

		Object& operator[](const std::string& aKey);
		Object& operator[](const char* aKey);

		Object& operator[](size_t aIndex);
		Object& operator[](int aIndex);
		Object& operator[](long aIndex);

		const Object& operator[](const std::string& aKey) const;
		const Object& operator[](const char* aKey) const;

		bool Has(const std::string& aKey) const;

		template<typename T>
		void AddValueChild(const std::string& aKey, T aValue);
		void AddChild(const std::string& aKey, Object* aChild);

		void MakeObject();
		void PushChild(Object* aChild);
		template<typename T>
		void PushValueChild(T aValue);
		void MakeArray();

		static Object& Null() { static Object null; return null; }
		bool NotNull() const { return !IsNull(); }
		bool IsNull() const { return this == &Null() || !myValue.has_value(); }

		std::string Serialize(bool aPretty = false);

		Object& operator=(const long long& aValue);
		Object& operator=(const long& aValue);
		Object& operator=(const size_t& aValue);
		Object& operator=(const int& aValue);
		Object& operator=(const double& aValue);
		Object& operator=(const float& aValue);
		Object& operator=(const std::string& aValue);
		Object& operator=(const char*& aValue);
		Object& operator=(const bool& aValue);

		bool operator !() const;
		operator bool() const;

		template<typename Type>
		bool Is() const;

		template<class Type>
		Type Get()
		{
			return static_cast<const Object* const>(this)->Get<Type>();
		};

		template<class Type>
		Type Get() const;

		template<typename Type>
		inline bool GetIf(Type& aValueToPlaceIn) const 
		{
			if (NotNull() && Is<Type>())
			{
				aValueToPlaceIn = Get<Type>();
				return true;
			}
			return false;
		}

		std::unordered_map<std::string, Object*>::iterator begin();
		std::unordered_map<std::string, Object*>::iterator end();

	private:

		void MakeValue();
		void CleanUpChildren();
		void ParseAsValue(const char* aBegin, const char* aEnd);

		enum class Type
		{
			None,
			Array,
			Object,
			Value
		};
		Type myType = Type::None;

		std::optional<
			std::variant<
				long long,
				double, 
				std::string, 
				bool,
				std::vector<Object*>,
				std::unordered_map<std::string, Object*>>> myValue;

	};

	class ArrayWrapper
	{
		friend Object;
	public:

		Object& operator[](size_t aIndex);
		DereferencingIteratorWrapper<std::vector<Object*>::iterator> begin();
		DereferencingIteratorWrapper<std::vector<Object*>::iterator> end();

		void PushChild(FiskJSON::Object* aObject);
		
		template<class T>
		void PushValue(T&& aValue);

	private:
		ArrayWrapper(Object* aParent);

		std::vector<Object*>* myArrayRef = nullptr;
	};

	class ConstArrayWrapper
	{
		friend Object;
	public:

		const Object& operator[](size_t aIndex);
		DereferencingIteratorWrapper<std::vector<Object*>::const_iterator> begin();
		DereferencingIteratorWrapper<std::vector<Object*>::const_iterator> end();

	private:
		ConstArrayWrapper(const Object* aParent);

		const std::vector<Object*>* myArrayRef = nullptr;
	};

#pragma region Is

#pragma region number
	template<>
	inline bool Object::Is<size_t>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 0;
	}
	template<>
	inline bool Object::Is<long long>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 0;
	}
	template<>
	inline bool Object::Is<long>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 0;
	}
	template<>
	inline bool Object::Is<int>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 0;
	}
#pragma endregion

#pragma region floatingPoint
	template<>
	inline bool Object::Is<float>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 1;
	}
	template<>
	inline bool Object::Is<double>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 1;
	}
#pragma endregion

#pragma region text
	template<>
	inline bool Object::Is<std::string>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 2;
	}
	template<>
	inline bool Object::Is<const char*>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 2;
	}
	template<>
	inline bool Object::Is<char*>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 2;
	}
#pragma endregion

#pragma region Array
	template<>
	inline bool Object::Is<std::vector<Object*>>() const
	{
		return NotNull() && myType == Type::Array;
	}
	template<>
	inline bool Object::Is<ArrayWrapper>() const
	{
		return NotNull() && myType == Type::Array;
	}
#pragma endregion

#pragma region Bool
	template<>
	inline bool Object::Is<bool>() const
	{
		return NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 3;
	}
#pragma endregion


#pragma endregion

#pragma region Get

#pragma region Number
	template<>
	inline size_t Object::Get<size_t>() const
	{
		if (!Is<size_t>())
		{
			throw Invalid_Get("object is not a number");
		}
		return std::get<long long>(myValue.value());
	}

	template<>
	inline long long Object::Get<long long>() const
	{
		if (!Is<long long>())
		{
			throw Invalid_Get("object is not a number");
		}
		return std::get<long long>(myValue.value());
	}

	template<>
	inline long Object::Get<long>() const
	{
		if (!Is<long long>())
		{
			throw Invalid_Get("object is not a number");
		}
		return long(std::get<long long>(myValue.value()));
	}


	template<>
	inline int Object::Get<int>() const
	{
		if (!Is<long long>())
		{
			throw Invalid_Get("object is not a number");
		}
		return int(std::get<long long>(myValue.value()));
	}

#pragma endregion

#pragma region FloatingPoint
	template<>
	inline double Object::Get<double>() const
	{
		if (!Is<double>())
		{
			throw Invalid_Get("object is not a floating point");
		}
		return std::get<double>(myValue.value());
	}

	template<>
	inline float Object::Get<float>() const
	{
		if (!Is<double>())
		{
			throw Invalid_Get("object is not a floating point");
		}
		return float(std::get<double>(myValue.value()));
	}
#pragma endregion

#pragma region Text
	template<>
	inline std::string Object::Get<std::string>() const
	{
		if (!Is<std::string>())
		{
			throw Invalid_Get("object is not text");
		}
		return std::get<std::string>(myValue.value());
	}
	template<>
	///This pointer will only be valid until the object changes
	inline const char* Object::Get<const char*>() const
	{
		if (!Is<std::string>())
		{
			throw Invalid_Get("object is not text");
		}
		return std::get<std::string>(myValue.value()).c_str();
	}
	template<>
	inline char* Object::Get<char*>() const
	{
		if (!Is<std::string>())
		{
			throw Invalid_Get("object is not text");
		}
		return const_cast<char*>(std::get<std::string>(myValue.value()).c_str());
	}
#pragma endregion

#pragma region Array
	template<>
	inline ArrayWrapper Object::Get<ArrayWrapper>()
	{
		return ArrayWrapper(this);
	}

	template<>
	inline ConstArrayWrapper Object::Get<ConstArrayWrapper>() const
	{
		return ConstArrayWrapper(this);
	}
#pragma endregion

#pragma region Bool
	template<>
	inline bool Object::Get<bool>() const
	{
		if (!Is<bool>())
		{
			throw Invalid_Get("object is not boolean");
		}
		return std::get<bool>(myValue.value());
	}
#pragma endregion

#pragma endregion

#pragma region Equals

#pragma region Number
	inline Object& Object::operator=(const long long& aValue)
	{
		if (IsNull()) { return Null(); }
		MakeValue();
		myValue = aValue;
		return *this;
	}
	inline Object& Object::operator=(const long& aValue)
	{
		return operator= (static_cast<long long>(aValue));
	}
	inline Object& Object::operator=(const size_t& aValue)
	{
		return operator= (static_cast<long long>(aValue));
	}

	inline Object& Object::operator=(const int& aValue)
	{
		return operator= (static_cast<long long>(aValue));
	}
#pragma endregion

#pragma region floatingPoint
	inline Object& Object::operator=(const double& aValue)
	{
		if ( IsNull() ) { return Null(); }
		MakeValue();
		myValue = double(aValue);
		return *this;
	}
	inline Object& Object::operator=(const float& aValue)
	{
		return operator= (double(aValue));
	}
#pragma endregion

#pragma region Text
	inline Object& Object::operator=(const std::string& aValue)
	{
		if (IsNull()) { return Null(); }
		MakeValue();
		myValue = aValue;
		return *this;
	}
	inline Object& Object::operator=(const char*& aValue)
	{
		return operator= (std::string(aValue));
	}
#pragma endregion

#pragma region bool

	inline Object& Object::operator=(const bool& aValue)
	{
		if (IsNull()) { return Null(); }
		MakeValue();
		myValue = aValue;
		return *this;
	}

	inline bool Object::operator!() const
	{
		return !operator bool();
	}

	inline Object::operator bool() const
	{
		return NotNull();
	}
#pragma endregion

#pragma endregion

	
	template<>
	inline bool Object::GetIf<float>(float& aValueToPlaceIn) const
	{
		if (Is<float>())
		{
			aValueToPlaceIn = Get<float>();
			return true;
		}
		if (Is<long long>())
		{
			aValueToPlaceIn = float(Get<long long>());
			return true;
		}
		return false;
	}

	template<typename T>
	inline void Object::AddValueChild(const std::string& aKey, T aValue)
	{
		Object* child = new Object();
		*child = aValue;
		AddChild(aKey, child);
	}	

	template<typename T>
	inline void Object::PushValueChild(T aValue)
	{
		Object* child = new Object();
		*child = aValue;
		PushChild(child);
	}
	
	template<class T>
	inline void ArrayWrapper::PushValue(T&& aValue)
	{
		if (!myArrayRef) { return; }

		FiskJSON::Object* obj = new FiskJSON::Object();
		*obj = std::forward<T>(aValue);
		myArrayRef->push_back(obj);
	}
}

#endif