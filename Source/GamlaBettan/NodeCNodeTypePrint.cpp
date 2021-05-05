#include "pch.h"
#include "NodeCNodeTypePrint.h"
#include "CNodeInstance.h"
#include <iostream>
#include "Logger.h"
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <sstream>
#include "NodeDataTypes.h"

CNodeTypePrint::CNodeTypePrint()
{
	myPins.push_back(CPin("IN"));
	myPins.push_back(CPin("OUT", CPin::PinTypeInOut::PinTypeInOut_OUT));
	myPins.push_back(CPin("Text", CPin::PinTypeInOut::PinTypeInOut_IN,CPin::PinType::Data));
	myPins.push_back(CPin("Type", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));

	SetPinType<std::string>(2);
	SetPinType<Logger::Type>(3);
}
namespace PrintNodeHelpers
{
	template<typename T>
	std::string AnyToString(std::any& aData)
	{
		T val = std::any_cast<T>(aData);
		std::stringstream data;
		data << "Data: ";
		data << val;
		return (data.str());
	}
	template<>
	std::string AnyToString<V3F>(std::any& aData)
	{
		V3F val = std::any_cast<V3F>(aData);
		std::stringstream data;
		data << "Data: ";
		data << val.x << " " << val.y << " " << val.z;
		return (data.str());
	}
	template<>
	std::string AnyToString<bool>(std::any& aData)
	{
		bool val = std::any_cast<bool>(aData);
		std::stringstream data;
		data << "Data: ";
		data << (data ? "true" : "false");
		return (data.str());
	}
}

int CNodeTypePrint::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia)
{
	static std::unordered_map<std::type_index, std::function<std::string(std::any&)>> myFunctionMapping;
	if (myFunctionMapping.empty())
	{
		myFunctionMapping[typeid(int)] = &PrintNodeHelpers::AnyToString<int>;
		myFunctionMapping[typeid(bool)] = &PrintNodeHelpers::AnyToString<bool>;
		myFunctionMapping[typeid(float)] = &PrintNodeHelpers::AnyToString<float>;
		myFunctionMapping[typeid(std::string)] = &PrintNodeHelpers::AnyToString<std::string>;
		myFunctionMapping[typeid(V3F)] = &PrintNodeHelpers::AnyToString<V3F>;

	}
	std::any data = aTriggeringNodeInstance->ReadRaw(2);
	Logger::Type type = Logger::Type::Info;
	aTriggeringNodeInstance->ReadData(3,type);
	if (myFunctionMapping.count(data.type()) != 0)
	{
		std::string message = myFunctionMapping[data.type()](data);
		Logger::Log(type, message);
	}
	else
	{
		SYSWARNING("Print Node does not support type",std::string(data.type().name()));
	}

	return 1;
}
