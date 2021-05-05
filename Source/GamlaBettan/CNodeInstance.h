#pragma once
#include "NodeTypes.h"
#include "CNodeType.h"
#include "CUID.h"
#include <vector>
#include <any>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <sstream>
#include "CGraphManager.h"
#include <unordered_map>
#include <functional>
#include "NodePollingStation.h"
#include "NodeDataTypes.h"

#define UNDEFINED_PIN_ID INT_MAX 
class GameObject;
struct SNodeInstanceLink
{
	SNodeInstanceLink(class CNodeInstance* aLink, unsigned int aFrom, unsigned int aTo, unsigned int aLinkID)
		:myLink(aLink), myFromPinID(aFrom), myToPinID(aTo), myLinkID(aLinkID)
	{

	}
	class CNodeInstance* myLink = nullptr;
	unsigned int myFromPinID = UNDEFINED_PIN_ID;
	unsigned int myToPinID = UNDEFINED_PIN_ID;
	unsigned int myLinkID = 0;
};

class CNodeInstance
{
public:
	CNodeInstance(bool aCreateNewUID = true);
	UID myUID;
	class CNodeType* myNodeType;
	void Enter(const int aEnteredVia);
	void ExitVia(unsigned int aPinIndex);
	void ConstructUniquePins();

	bool CanAddLink(unsigned int aPinIdFromMe);
	bool AddLinkToVia(CNodeInstance* aLink, unsigned int aPinIdFromMe, unsigned int aPinIdToMe, unsigned int aLinkID);
	void RemoveLinkToVia(CNodeInstance* aLink, unsigned int aPinThatIOwn);


	bool IsPinConnected(CPin& aPin);
	std::string GetNodeName();

	std::vector<CPin>& GetPins() { return myPins; }
	void ChangePinTypes(CPin::PinType aType);

	int GetLocalPinID(const int anID);

	std::any ReadRaw(unsigned int aPinIndex);
	template<typename T>
	_NODISCARD const bool ReadData(unsigned int aPinIndex,T& outData);

	template<typename T>
	const bool WriteData(unsigned int aPinIndex, T aData);

	std::vector<SNodeInstanceLink>& GetLinks() { return myLinks; }
	std::vector<SNodeInstanceLink*> GetLinksFromPin(unsigned int aPinToFetchFrom);
	CPin* GetPinFromID(unsigned int aUID);
	int GetPinIndexFromPinUID(unsigned int aPinUID);

	inline std::string WriteVariableType(const CPin& aPin) const
	{
		static std::unordered_map<std::type_index, std::string> nameMapping;
		if (nameMapping.empty())
		{
			nameMapping[typeid(void)] = "NOTHING";
			nameMapping[typeid(bool)] = "BOOL";
			nameMapping[typeid(int)] = "INT";
			nameMapping[typeid(float)] = "FLOAT";
			nameMapping[typeid(std::string)] = "STRING";
			nameMapping[typeid(EntityType)] = "ENTITYTYPE";
			nameMapping[typeid(V3F)] = "VECTOR3";
			nameMapping[typeid(GameObjectId)] = "ENTITYID";
			nameMapping[typeid(Logger::Type)] = "LOGGERTYPE";
		}
		if (nameMapping.count(aPin.myData.type()) != 0)
		{
			return nameMapping[aPin.myData.type()];
		}
		else
		{
			SYSWARNING("Could not save data of type", std::string(aPin.myData.type().name()))
		}
		return "UNKOWN";
	}


	template <typename Writer>
	inline void WritePinValue(Writer& writer, const CPin& aPin) const
	{

		static std::unordered_map<std::type_index, std::function<void(Writer&, const CPin&)>> writeMap;
		if (writeMap.empty())
		{
			writeMap[typeid(void)]			= [](Writer& writer, const CPin& aPin) -> void { writer.String("");												};
			writeMap[typeid(bool)]			= [](Writer& writer, const CPin& aPin) -> void { writer.Bool(std::any_cast<bool>(aPin.myData));					};
			writeMap[typeid(int)]			= [](Writer& writer, const CPin& aPin) -> void { writer.Int(std::any_cast<int>(aPin.myData));					};
			writeMap[typeid(float)]			= [](Writer& writer, const CPin& aPin) -> void { writer.Double(std::any_cast<float>(aPin.myData));				};
			writeMap[typeid(std::string)]	= [](Writer& writer, const CPin& aPin) -> void { writer.String(std::any_cast<std::string>(aPin.myData).c_str());};
			writeMap[typeid(EntityType)]	= [](Writer& writer, const CPin& aPin) -> void { writer.Int(static_cast<int>(std::any_cast<EntityType>(aPin.myData))); };
			writeMap[typeid(GameObjectId)]	= [](Writer& writer, const CPin& aPin) -> void { writer.String("");												};
			writeMap[typeid(TimerId)] = [](Writer& writer, const CPin& aPin) -> void { writer.String("");												};
			writeMap[typeid(Logger::Type)]	= [](Writer& writer, const CPin& aPin) -> void { writer.Uint64(static_cast<unsigned long long>(std::any_cast<Logger::Type>(aPin.myData)));};
			writeMap[typeid(V3F)]			= [](Writer& writer, const CPin& aPin) -> void 
			{
				V3F data = std::any_cast<V3F>(aPin.myData);
				writer.StartArray();
				writer.Double(data.x);
				writer.Double(data.y);
				writer.Double(data.z);
				writer.EndArray();
			};
		}

		if (aPin.myPinType == CPin::PinTypeInOut::PinTypeInOut_OUT)
		{
			writer.String("");
		}
		else
		{
			if (writeMap.count(aPin.myData.type()) != 0)
			{
				writeMap[aPin.myData.type()](writer, aPin);
			}
			else
			{
				SYSERROR("Type writing is not supported",std::string(aPin.myData.type().name()));
			}
		}
	}

	template <typename Writer>
	void Serialize(Writer& writer) const
	{
		writer.StartObject();
		writer.Key("NodeType");
		writer.Int(myNodeType->myID);

		writer.Key("UID");
		writer.Int(myUID.AsInt());

		ImVec2 peditorPos = ed::GetNodePosition(myUID.AsInt());
		writer.Key("Position");
		writer.StartObject();
		{
			writer.Key("X");
			writer.Int(CAST(int, peditorPos.x));
			writer.Key("Y");
			writer.Int(CAST(int, peditorPos.y));
		}
		writer.EndObject();

		writer.Key("Pins");
		writer.StartArray();
		for (int i = 0; i < myPins.size(); i++)
		{
			//writer.Key("Pin");
			writer.StartObject();
			{
				writer.Key("Index");
				writer.Int(i);
				writer.Key("UID");
				writer.Int(myPins[i].myUID.AsInt());
				writer.Key("DATA");
				WritePinValue(writer, myPins[i]);
				writer.Key("DATA_TYPE");
				writer.String(WriteVariableType(myPins[i]).c_str());
			}
			writer.EndObject();

		}
		writer.EndArray();


		writer.EndObject();
	}

	void DebugUpdate();
	void VisualUpdate(float aTimeDelta);
	float myEditorPos[2];
	bool myHasSetEditorPos = false;
	bool myShouldTriggerAgain = false;
	std::unordered_map<std::string, std::any> myMetaData;
	std::vector<SNodeInstanceLink> myLinks;
	std::vector<CPin> myPins;
	

	virtual int GetColor()
	{
		if (myEnteredTimer > 0.0f)
		{
			if (myEnteredTimer < 1.0f)
			{
				return COL32(0, myEnteredTimer * 255, 0, 255);
			}
			else
			{
				return COL32(0, 255, 0, 255);
			}
		}
		if (myNodeType->IsStartNode())
		{
			return COL32(255, 128, 128, 255);
		}
		if (myNodeType->IsFlowNode())
		{
			return COL32(128, 195, 248, 255);
		}
		return COL32(255, 255, 255, 255);
	}
	float myEnteredTimer = 0.0f;

	static NodePollingStation* ourPollingStation;
private:

};

template<typename T>
inline const bool CNodeInstance::ReadData(unsigned int aPinIndex,T& outData)
{
	std::any data = ReadRaw(aPinIndex);
	if (data.type() == typeid(T))
	{
		outData = std::any_cast<T>(data);
		return true;
	}
	return false;
}

template<typename T>
inline const bool CNodeInstance::WriteData(unsigned int aPinIndex, T aData)
{
	if (aPinIndex < 0 || aPinIndex >= myPins.size())
	{
		return false;
	}

	CPin& dataPin = myPins[aPinIndex];
	// We have the data, set ut to the return values
	if (dataPin.myPinType == CPin::PinTypeInOut::PinTypeInOut_OUT)
	{
		dataPin.myData = aData;
		return true;
	}
	return false;
}
