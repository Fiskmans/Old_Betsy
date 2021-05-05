#pragma once
#include <string>
#include <unordered_map>
#include <optional>
#include <variant>
#include <vector>

namespace FiskJSON
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
		Invalid_Object(const std::string& aMessage) : std::exception(("This object has been severly invalidated: " + aMessage).c_str()) {}
	};

	class Invalid_Get: public std::exception
	{
	public:
		Invalid_Get(const std::string& aMessage) : std::exception(aMessage.c_str()) {}
	};

	void replaceAll(std::string& str, const std::string& from, const std::string& to);
	
	class Object
	{
		friend class Array;
	public:
		void Parse(const std::string& aDocument);

		~Object();

		Object& operator[](const std::string& aKey);
		Object& operator[](const char* aKey);
		Object& operator[](size_t aIndex);
		Object& operator[](int aIndex);
		Object& operator[](long aIndex);
		bool Has(const std::string& aKey);
		void AddChild(const std::string& aKey, Object* aChild);
		template<typename T>
		void AddValueChild(const std::string& aKey, T aValue);

		void MakeObject();
		void PushChild(Object* aChild);
		template<typename T>
		void PushValueChild(T aValue);
		void MakeArray();

		std::string Serialize(bool aPretty = false);

		std::vector<std::exception*> GetExceptions();

		Object& operator=(const long long& aValue);
		Object& operator=(const long& aValue);
		Object& operator=(const size_t& aValue);
		Object& operator=(const int& aValue);
		Object& operator=(const double& aValue);
		Object& operator=(const float& aValue);
		Object& operator=(const std::string& aValue);
		Object& operator=(const char*& aValue);
		Object& operator=(const bool& aValue);

		operator bool();

		template<typename Type>
		bool Is();

		template<class Type>
		Type Get();

		template<typename Type>
		inline bool GetIf(Type& aValueToPlaceIn)
		{
			if (Is<Type>() && this)
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
		void ParseAsValue(const std::string& aValue);

		enum class Type
		{
			None,
			Array,
			Object,
			Value
		};
		Type myType = Type::None;
		std::unordered_map<std::string, Object*> myChildren;
		std::optional<std::variant<long long, double, std::string, bool,std::vector<Object*>>> myValue;
		std::vector<std::exception*> myExceptions;
	};

	class Array
	{
		friend Object;
	public:

		Object& operator[](size_t aIndex);
		std::vector<Object*>::iterator begin();
		std::vector<Object*>::iterator end();

	private:
		Array(Object* aParent);

		std::vector<Object*>* myArrayRef = nullptr;
	};

#pragma region Is

#pragma region number
	template<>
	inline bool Object::Is<long long>()
	{
		return  this && myType == Type::Value && myValue.has_value() && myValue.value().index() == 0;
	}
	template<>
	inline bool Object::Is<long>()
	{
		return  this && myType == Type::Value && myValue.has_value() && myValue.value().index() == 0;
	}
	template<>
	inline bool Object::Is<int>()
	{
		return  this && myType == Type::Value && myValue.has_value() && myValue.value().index() == 0;
	}
#pragma endregion

#pragma region floatingPoint
	template<>
	inline bool Object::Is<float>()
	{
		return  this && myType == Type::Value && myValue.has_value() && myValue.value().index() == 1;
	}
	template<>
	inline bool Object::Is<double>()
	{
		return  this && myType == Type::Value && myValue.has_value() && myValue.value().index() == 1;
	}
#pragma endregion

#pragma region text
	template<>
	inline bool Object::Is<std::string>()
	{
		return  this && myType == Type::Value && myValue.has_value() && myValue.value().index() == 2;
	}
	template<>
	inline bool Object::Is<const char*>()
	{
		return  this && myType == Type::Value && myValue.has_value() && myValue.value().index() == 2;
	}
	template<>
	inline bool Object::Is<char*>()
	{
		return  this && myType == Type::Value && myValue.has_value() && myValue.value().index() == 2;
	}
#pragma endregion

#pragma region Array
	template<>
	inline bool Object::Is<std::vector<Object*>>()
	{
		return this && myType == Type::Array;
	}
	template<>
	inline bool Object::Is<Array>()
	{
		return  this && myType == Type::Array;
	}
#pragma endregion

#pragma region Bool
	template<>
	inline bool Object::Is<bool>()
	{
		return  this && myType == Type::Value && myValue.has_value() && myValue.value().index() == 3;
	}
#pragma endregion


#pragma endregion

#pragma region Get

#pragma region Number
	template<>
	inline long long Object::Get<long long>()
	{
		if (!Is<long long>())
		{
			throw Invalid_Get("[" + Serialize(true) + "] object is not a number");
		}
		return std::get<long long>(myValue.value());
	}

	template<>
	inline long Object::Get<long>()
	{
		if (!Is<long long>())
		{
			throw Invalid_Get("[" + Serialize(true) + "] object is not a number");
		}
		return long(std::get<long long>(myValue.value()));
	}


	template<>
	inline int Object::Get<int>()
	{
		if (!Is<long long>())
		{
			throw Invalid_Get("[" + Serialize(true) + "] object is not a number");
		}
		return int(std::get<long long>(myValue.value()));
	}

#pragma endregion

#pragma region FloatingPoint
	template<>
	inline double Object::Get<double>()
	{
		if (!Is<double>())
		{
			throw Invalid_Get("[" + Serialize(true) + "] object is not a floating point");
		}
		return std::get<double>(myValue.value());
	}

	template<>
	inline float Object::Get<float>()
	{
		if (!Is<double>())
		{
			throw Invalid_Get("[" + Serialize(true) + "] object is not a floating point");
		}
		return float(std::get<double>(myValue.value()));
	}
#pragma endregion

#pragma region Text
	template<>
	inline std::string Object::Get<std::string>()
	{
		if (!Is<std::string>())
		{
			throw Invalid_Get("[" + Serialize(true) + "] object is not text");
		}
		return std::get<std::string>(myValue.value());
	}
	template<>
	///This pointer will only be valid until the object changes
	inline const char* Object::Get<const char*>() 
	{
		if (!Is<std::string>())
		{
			throw Invalid_Get("[" + Serialize(true) + "] object is not text");
		}
		return std::get<std::string>(myValue.value()).c_str();
	}
	template<>
	inline char* Object::Get<char*>()
	{
#if !defined(FISKJSON_DISABLE_CHARPOINTER_WARNING)
		static_assert(true, "This could be really dangerous, define FISKJSON_DISABLE_CHARPOINTER_WARNING before including FiskJSON.h to disable warning");
#endif
		if (!Is<std::string>())
		{
			throw Invalid_Get("[" + Serialize(true) + "] object is not text");
		}
		return const_cast<char*>(std::get<std::string>(myValue.value()).c_str());
	}
#pragma endregion

#pragma region Array
	template<>
	inline Array Object::Get<Array>()
	{
		return Array(this);
	}
#pragma endregion

#pragma region Bool
	template<>
	inline bool Object::Get<bool>()
	{
		if (!Is<bool>())
		{
			throw Invalid_Get("[" + Serialize(true) + "] object is not boolean");
		}
		return std::get<bool>(myValue.value());
	}
#pragma endregion

#pragma endregion

#pragma region Equals

#pragma region Number
	inline Object& Object::operator=(const long long& aValue)
	{
		if (!this) { return *this; }
		MakeValue();
		myValue = aValue;
		return *this;
	}
	inline Object& Object::operator=(const long& aValue)
	{
		return operator= (long long(aValue));
	}
	inline Object& Object::operator=(const size_t& aValue)
	{
		return operator= (long long(aValue));
	}

	inline Object& Object::operator=(const int& aValue)
	{
		return operator= (long long(aValue));
	}
#pragma endregion

#pragma region floatingPoint
	inline Object& Object::operator=(const double& aValue)
	{
		if (!this) { return *this; }
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
		if (!this) { return *this; }
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
		if (!this) { return *this; }
		MakeValue();
		myValue = aValue;
		return *this;
	}
	inline Object::operator bool()
	{
		return !!this;
	}
#pragma endregion

#pragma endregion

	
	template<>
	inline bool Object::GetIf<float>(float& aValueToPlaceIn)
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
	template<>
	inline bool Object::GetIf<V3F>(V3F& aValueToPlaceIn)
	{
		if (Is<FiskJSON::Array>())
		{
			float data[3];
			char count = 0;
			for (auto& arrChild : Get<FiskJSON::Array>())
			{
				if (arrChild->GetIf(data[count]))
				{
					if (++count == 3)
					{
						aValueToPlaceIn = V3F(data[0], data[1], data[2]);
						return true;
					}
				}
			}
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

	template<>
	inline void Object::AddValueChild(const std::string& aKey, V3F aValue)
	{
		Object* child = new Object();
		child->MakeArray();
		for (auto& i : aValue)
		{
			child->PushValueChild(i);
		}
		AddChild(aKey, child);
	}
	

	template<typename T>
	inline void Object::PushValueChild(T aValue)
	{
		Object* child = new Object();
		*child = aValue;
		PushChild(child);
	}
}

