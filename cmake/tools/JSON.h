
#ifndef TOOLS_JSON_H
#define TOOLS_JSON_H

#include "tools/DereferencingIteratorWrapper.h"

#include <string>
#include <unordered_map>
#include <optional>
#include <variant>
#include <vector>

namespace tools
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

	class Invalid_JSONObject : public std::exception
	{
	public:
		Invalid_JSONObject(const std::string& aMessage) : std::exception(("This object has been severely invalidated: " + aMessage).c_str()) {}
	};

	class Invalid_Get : public std::exception
	{
	public:
		Invalid_Get(const std::string& aMessage) : std::exception(aMessage.c_str()) {}
	};

	void replaceAll(std::string& str, const std::string& from, const std::string& to);

	class JSONObject
	{
		friend class ArrayWrapper;
		friend class ConstArrayWrapper;
	public:
		JSONObject& Parse(const std::string& aDocument);
		JSONObject& Parse(const char* aBegin, const char* aEnd);

		~JSONObject();
		JSONObject() = default;
		JSONObject(const JSONObject& aOther) = delete;

		JSONObject& operator[](const std::string& aKey);
		JSONObject& operator[](const char* aKey);

		JSONObject& operator[](size_t aIndex);
		JSONObject& operator[](int aIndex);
		JSONObject& operator[](long aIndex);

		const JSONObject& operator[](const std::string& aKey) const;
		const JSONObject& operator[](const char* aKey) const;

		bool Has(const std::string& aKey) const;

		template<typename T>
		void AddValueChild(const std::string& aKey, T aValue);
		void AddChild(const std::string& aKey, JSONObject* aChild);

		void MakeObject();
		void PushChild(JSONObject* aChild);
		template<typename T>
		void PushValueChild(T aValue);
		void MakeArray();

		static JSONObject& Null() { static JSONObject null; return null; }
		bool NotNull() const { return !IsNull(); }
		bool IsNull() const { return this == &Null() || !myValue.has_value(); }

		std::string Serialize(bool aPretty = false);

		JSONObject& operator=(const long long& aValue);
		JSONObject& operator=(const long& aValue);
		JSONObject& operator=(const size_t& aValue);
		JSONObject& operator=(const int& aValue);
		JSONObject& operator=(const double& aValue);
		JSONObject& operator=(const float& aValue);
		JSONObject& operator=(const std::string& aValue);
		JSONObject& operator=(const char*& aValue);
		JSONObject& operator=(const bool& aValue);

		bool operator !() const;
		operator bool() const;

		template<typename Type>
		bool Is() const;

		template<class Type>
		Type Get()
		{
			return static_cast<const JSONObject* const>(this)->Get<Type>();
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

		std::unordered_map<std::string, JSONObject*>::iterator begin();
		std::unordered_map<std::string, JSONObject*>::iterator end();

	private:

		void MakeValue();
		void CleanUpChildren();
		void ParseAsValue(const char* aBegin, const char* aEnd);

		enum class Type
		{
			None,
			Array,
			JSONObject,
			Value
		};
		Type myType = Type::None;

		std::optional<
			std::variant<
			long long,
			double,
			std::string,
			bool,
			std::vector<JSONObject*>,
			std::unordered_map<std::string, JSONObject*>>> myValue;

	};

	class ArrayWrapper
	{
		friend JSONObject;
	public:

		JSONObject& operator[](size_t aIndex);
		DereferencingIteratorWrapper<std::vector<JSONObject*>::iterator> begin();
		DereferencingIteratorWrapper<std::vector<JSONObject*>::iterator> end();

		void PushChild(JSONObject* aJSONObject);

		template<class T>
		void PushValue(T&& aValue);

	private:
		ArrayWrapper(JSONObject* aParent);

		std::vector<JSONObject*>* myArrayRef = nullptr;
	};

	class ConstArrayWrapper
	{
		friend JSONObject;
	public:

		const JSONObject& operator[](size_t aIndex);
		DereferencingIteratorWrapper<std::vector<JSONObject*>::const_iterator> begin();
		DereferencingIteratorWrapper<std::vector<JSONObject*>::const_iterator> end();

	private:
		ConstArrayWrapper(const JSONObject* aParent);

		const std::vector<JSONObject*>* myArrayRef = nullptr;
	};

#pragma region Is

#pragma region number
	template<>
	inline bool JSONObject::Is<size_t>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 0;
	}
	template<>
	inline bool JSONObject::Is<long long>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 0;
	}
	template<>
	inline bool JSONObject::Is<long>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 0;
	}
	template<>
	inline bool JSONObject::Is<int>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 0;
	}
#pragma endregion

#pragma region floatingPoint
	template<>
	inline bool JSONObject::Is<float>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 1;
	}
	template<>
	inline bool JSONObject::Is<double>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 1;
	}
#pragma endregion

#pragma region text
	template<>
	inline bool JSONObject::Is<std::string>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 2;
	}
	template<>
	inline bool JSONObject::Is<const char*>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 2;
	}
	template<>
	inline bool JSONObject::Is<char*>() const
	{
		return  NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 2;
	}
#pragma endregion

#pragma region Array
	template<>
	inline bool JSONObject::Is<std::vector<JSONObject*>>() const
	{
		return NotNull() && myType == Type::Array;
	}
	template<>
	inline bool JSONObject::Is<ArrayWrapper>() const
	{
		return NotNull() && myType == Type::Array;
	}
#pragma endregion

#pragma region Bool
	template<>
	inline bool JSONObject::Is<bool>() const
	{
		return NotNull() && myType == Type::Value && myValue.has_value() && myValue.value().index() == 3;
	}
#pragma endregion


#pragma endregion

#pragma region Get

#pragma region Number
	template<>
	inline size_t JSONObject::Get<size_t>() const
	{
		if (!Is<size_t>())
		{
			throw Invalid_Get("object is not a number");
		}
		return std::get<long long>(myValue.value());
	}

	template<>
	inline long long JSONObject::Get<long long>() const
	{
		if (!Is<long long>())
		{
			throw Invalid_Get("object is not a number");
		}
		return std::get<long long>(myValue.value());
	}

	template<>
	inline long JSONObject::Get<long>() const
	{
		if (!Is<long long>())
		{
			throw Invalid_Get("object is not a number");
		}
		return long(std::get<long long>(myValue.value()));
	}


	template<>
	inline int JSONObject::Get<int>() const
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
	inline double JSONObject::Get<double>() const
	{
		if (!Is<double>())
		{
			throw Invalid_Get("object is not a floating point");
		}
		return std::get<double>(myValue.value());
	}

	template<>
	inline float JSONObject::Get<float>() const
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
	inline std::string JSONObject::Get<std::string>() const
	{
		if (!Is<std::string>())
		{
			throw Invalid_Get("object is not text");
		}
		return std::get<std::string>(myValue.value());
	}
	template<>
	///This pointer will only be valid until the object changes
	inline const char* JSONObject::Get<const char*>() const
	{
		if (!Is<std::string>())
		{
			throw Invalid_Get("object is not text");
		}
		return std::get<std::string>(myValue.value()).c_str();
	}
	template<>
	inline char* JSONObject::Get<char*>() const
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
	inline ArrayWrapper JSONObject::Get<ArrayWrapper>()
	{
		return ArrayWrapper(this);
	}

	template<>
	inline ConstArrayWrapper JSONObject::Get<ConstArrayWrapper>() const
	{
		return ConstArrayWrapper(this);
	}
#pragma endregion

#pragma region Bool
	template<>
	inline bool JSONObject::Get<bool>() const
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
	inline JSONObject& JSONObject::operator=(const long long& aValue)
	{
		if (IsNull()) { return Null(); }
		MakeValue();
		myValue = aValue;
		return *this;
	}
	inline JSONObject& JSONObject::operator=(const long& aValue)
	{
		return operator= (static_cast<long long>(aValue));
	}
	inline JSONObject& JSONObject::operator=(const size_t& aValue)
	{
		return operator= (static_cast<long long>(aValue));
	}

	inline JSONObject& JSONObject::operator=(const int& aValue)
	{
		return operator= (static_cast<long long>(aValue));
	}
#pragma endregion

#pragma region floatingPoint
	inline JSONObject& JSONObject::operator=(const double& aValue)
	{
		if (IsNull()) { return Null(); }
		MakeValue();
		myValue = double(aValue);
		return *this;
	}
	inline JSONObject& JSONObject::operator=(const float& aValue)
	{
		return operator= (double(aValue));
	}
#pragma endregion

#pragma region Text
	inline JSONObject& JSONObject::operator=(const std::string& aValue)
	{
		if (IsNull()) { return Null(); }
		MakeValue();
		myValue = aValue;
		return *this;
	}
	inline JSONObject& JSONObject::operator=(const char*& aValue)
	{
		return operator= (std::string(aValue));
	}
#pragma endregion

#pragma region bool

	inline JSONObject& JSONObject::operator=(const bool& aValue)
	{
		if (IsNull()) { return Null(); }
		MakeValue();
		myValue = aValue;
		return *this;
	}

	inline bool JSONObject::operator!() const
	{
		return !operator bool();
	}

	inline JSONObject::operator bool() const
	{
		return NotNull();
	}
#pragma endregion

#pragma endregion


	template<>
	inline bool JSONObject::GetIf<float>(float& aValueToPlaceIn) const
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
	inline void JSONObject::AddValueChild(const std::string& aKey, T aValue)
	{
		JSONObject* child = new JSONObject();
		*child = aValue;
		AddChild(aKey, child);
	}

	template<typename T>
	inline void JSONObject::PushValueChild(T aValue)
	{
		JSONObject* child = new JSONObject();
		*child = aValue;
		PushChild(child);
	}

	template<class T>
	inline void ArrayWrapper::PushValue(T&& aValue)
	{
		if (!myArrayRef) { return; }

		FiskJSON::JSONObject* obj = new FiskJSON::JSONObject();
		*obj = std::forward<T>(aValue);
		myArrayRef->push_back(obj);
	}
}

#endif