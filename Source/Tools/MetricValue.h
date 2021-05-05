#pragma once
#if !BAKEVALUES
#pragma warning(push)
#pragma warning(disable : 26495)

#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <imgui.h>
#include <Vector3.hpp>

class SerializableMetricValue;

class MetricHandler
{
public:
	static void Load(std::string aFilePath);
	static void Save(std::string aFilePath);
	static void Apply(SerializableMetricValue* aValue);
	static void Unload();

	static void StartListening(const std::string& aLookup, SerializableMetricValue* aTarget);
	static void CopySubscription(const SerializableMetricValue* aFrom, SerializableMetricValue* aTo);
	static void StopListening(SerializableMetricValue* aTarget);

	static void ImGuiValueEditor();

private:
	static std::vector<std::pair<std::string, SerializableMetricValue*>> myListeners;
	static std::unordered_map<std::string, std::string> myLoadedMetrics;
};





class SerializableMetricValue
{
public:
	SerializableMetricValue();
	void Setup(const std::string& aLookup);
	SerializableMetricValue(const SerializableMetricValue& aOther);
	~SerializableMetricValue();
	virtual bool EditImGui() = 0;
	virtual std::string Serialize() = 0;
	virtual void DeSerialize(std::string& aString) = 0;
};


template<class Type>
class MetricValue : public SerializableMetricValue
{
	bool myAssigned = false;
	Type myValue;
public:
	MetricValue(const std::string& aLookup, Type aDefaultValue);
	operator Type& () { return myValue; }

	// Inherited via SerializableMetricValue
	virtual std::string Serialize() override
	{
		std::stringstream stream;
		stream << myValue;
		return stream.str();
	}
	virtual void DeSerialize(std::string& aString) override
	{
		myAssigned = true;
		std::stringstream stream(aString);
		stream >> myValue;
	}
};

template<class Type>
inline MetricValue<Type>::MetricValue(const std::string& aLookup, Type aDefaultValue) : SerializableMetricValue(aLookup)
{
	if (!myAssigned)
	{
		myValue = aDefaultValue;
	}
}

#pragma region Specializations
#pragma region float

template<>
class MetricValue<float> : public SerializableMetricValue
{
	bool myAssigned = false;
	float myValue;
public:
	MetricValue(const std::string& aLookup, float aDefaultValue);
	operator float& () { return myValue; }

	// Inherited via SerializableMetricValue
	virtual std::string Serialize() override 
	{
		std::stringstream stream;
		stream << myValue;
		return stream.str();
	}
	virtual void DeSerialize(std::string& aString) override
	{
		myAssigned = true;
		std::stringstream stream(aString);
		stream >> myValue;
	}

	// Inherited via SerializableMetricValue
	virtual bool EditImGui() override
	{
		ImGui::SameLine();
		return ImGui::DragFloat("",&myValue,0.01f);
	}
};
inline MetricValue<float>::MetricValue(const std::string& aLookup, float aDefaultValue)
{
	SerializableMetricValue::Setup(aLookup);
	if (!myAssigned)
	{
		myValue = aDefaultValue;
	}
}

#pragma endregion

#pragma region bool

template<>
class MetricValue<bool> : public SerializableMetricValue
{
	bool myAssigned = false;
	bool myValue;
public:
	MetricValue(const std::string& aLookup, bool aDefaultValue);
	operator bool& () { return myValue; }

	// Inherited via SerializableMetricValue
	virtual std::string Serialize() override
	{
		std::stringstream stream;
		stream << (myValue ? "true" : "false");
		return stream.str();
	}
	virtual void DeSerialize(std::string& aString) override
	{
		myAssigned = true;
		myValue = aString == "true";
	}

	// Inherited via SerializableMetricValue
	virtual bool EditImGui() override
	{
		ImGui::SameLine();
		return ImGui::Checkbox("", &myValue);
	}
};
inline MetricValue<bool>::MetricValue(const std::string& aLookup, bool aDefaultValue)
{
	SerializableMetricValue::Setup(aLookup);
	if (!myAssigned)
	{
		myValue = aDefaultValue;
	}
}
#pragma endregion

#pragma region int

template<>
class MetricValue<int> : public SerializableMetricValue
{
	bool myAssigned = false;
	int myValue;
public:
	MetricValue(const std::string& aLookup, int aDefaultValue);
	operator int& () { return myValue; }

	// Inherited via SerializableMetricValue
	virtual std::string Serialize() override
	{
		std::stringstream stream;
		stream << myValue;
		return stream.str();
	}
	virtual void DeSerialize(std::string& aString) override
	{
		myAssigned = true;
		std::stringstream stream(aString);
		stream >> myValue;
	}

	// Inherited via SerializableMetricValue
	virtual bool EditImGui() override
	{
		ImGui::SameLine();
		return ImGui::DragInt("", &myValue, 0.01f);
	}
};

inline MetricValue<int>::MetricValue(const std::string& aLookup, int aDefaultValue)
{
	SerializableMetricValue::Setup(aLookup);
	if (!myAssigned)
	{
		myValue = aDefaultValue;
	}
}
#pragma endregion

#pragma region v3f

template<>
class MetricValue<V3F> : public SerializableMetricValue
{
	bool myAssigned = false;
	V3F myValue;
public:
	MetricValue(const std::string& aLookup, V3F aDefaultValue);
	operator V3F& () { return myValue; }

	// Inherited via SerializableMetricValue
	virtual std::string Serialize() override
	{
		std::stringstream stream;
		stream << myValue.x << "," << myValue.y << "," << myValue.z;
		return stream.str();
	}
	virtual void DeSerialize(std::string& aString) override
	{
		myAssigned = true;
		std::stringstream ss(aString);
		ss >> myValue.x;
		ss.get();
		ss >> myValue.y;
		ss.get();
		ss >> myValue.z;
	}

	// Inherited via SerializableMetricValue
	virtual bool EditImGui() override
	{
		ImGui::SameLine();
		return ImGui::ColorEdit3("", &myValue.x);
	}
};
inline MetricValue<V3F>::MetricValue(const std::string& aLookup, V3F aDefaultValue)
{
	SerializableMetricValue::Setup(aLookup);
	if (!myAssigned)
	{
		myValue = aDefaultValue;
	}
}

#pragma endregion


#pragma endregion




#endif // !_RETAIL
#pragma warning(pop)
