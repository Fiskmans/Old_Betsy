#pragma once
#include "NodeTypes.h"
#include "CUID.h"
#include <any>

class UID
{

public:
	UID(bool aCreateNewUID = true) : myID(INT_MAX)
	{
		if (!aCreateNewUID)
		{
			return;
		}
		myGlobalUID++;
		myID = myGlobalUID;
		while (std::find(myAllUIDs.begin(), myAllUIDs.end(), myID) != myAllUIDs.end())
		{
			// Print warning, ID already in use :(
			myGlobalUID++;
			myID = myGlobalUID;
		}
		myAllUIDs.push_back(myID);
	}

	const unsigned int AsInt() const {return myID;}

	UID& operator=(const UID& other)
	{
		myID = other.myID;
#ifdef _DEBUG
		if (std::find(myAllUIDs.begin(), myAllUIDs.end(), myID) != myAllUIDs.end())
		{
			assert(0);
		}
#endif
		return *this;
	}
	UID& operator=(const int other)
	{
		myID = other;
#ifdef _DEBUG
		if (std::find(myAllUIDs.begin(), myAllUIDs.end(), myID) != myAllUIDs.end())
		{
			assert(0);
		}
#endif
		return *this;
	}

	void SetUID(unsigned int aID)
	{
		myID = aID;
	}

	static std::vector<unsigned int> myAllUIDs;
	static unsigned int myGlobalUID;
private:
	unsigned int myID;

};


struct CPin
{

	enum class PinType
	{
		Flow,
		Data,
		Unknown
	};

	enum class PinTypeInOut
	{
		PinTypeInOut_IN,
		PinTypeInOut_OUT
	};

	CPin(std::string aText, PinTypeInOut aType = PinTypeInOut::PinTypeInOut_IN, PinType aVarType = PinType::Flow)
		:myText(aText)
	{
		myVariableType = aVarType;
		myPinType = aType;
	}

	CPin(const CPin& p2)
	{
		myText = p2.myText;
		myVariableType = p2.myVariableType;
		myPinType = p2.myPinType;
		myData = p2.myData;
		//myUID.SetUID(p2.myUID.AsInt()); // Cant do this here, copy constructor should create new UID and it will if we dont set it here
	}

	CPin& operator=(const CPin& p2)
	{
		myText = p2.myText;
		myVariableType = p2.myVariableType;
		myPinType = p2.myPinType;
		myData = p2.myData;
		myUID.SetUID(p2.myUID.AsInt()); // = operator should use the UID the last pin had, no new UID here, only in copy constructor
	}

	std::string myText;
	UID myUID;
	std::any myData;
	PinType myVariableType = PinType::Flow;
	PinTypeInOut myPinType;
};



class CNodeType
{
public:

	int DoEnter(class CNodeInstance* aTriggeringNodeInstance, const int aEnteredVia);
	virtual std::string GetNodeName() { return "N/A"; }

	std::vector<CPin> GetPins();
	template<class T>
	void SetPinType(unsigned int aPinIndex);

	virtual bool IsStartNode() { return false; }
	virtual std::string GetNodeTypeCategory() { return ""; }
	bool IsFlowNode()
	{
		for (auto& pin : myPins)
		{
			if (pin.myVariableType == CPin::PinType::Flow)
			{
				return true;
			}
		}
		return false;
	}
	// Draw debug? Draw text?
	virtual void DebugUpdate(class CNodeInstance*) {}

	int myID = -1;
	std::string mySemanticName;
	
protected:
	template <class T>
	void DeclareDataOnPinIfNecessary(CPin& aPin)
	{
		if (!aPin.myData.has_value())
		{
			aPin.myData = new T;
		}
		
	}
	virtual int OnEnter(class CNodeInstance* /*aTriggeringNodeInstance*/, const int aEnteredVia){return -1;};

	std::vector<CPin> myPins;
	


};

class CNodeTypeCollector
{
public:
	static void PopulateTypes();
	static CNodeType* GetNodeTypeFromID(unsigned int aClassID)
	{
		return myTypes[aClassID]; // 1:1 to nodetype enum
	}
	static CNodeType** GetAllNodeTypes()
	{
		return myTypes; // 1:1 to nodetype enum
	}
	static unsigned short GetNodeTypeCount()
	{
		return 	myTypeCounter; // 1:1 to nodetype enum
	}
	template <class T>
	static void RegisterType()
	{
		myTypes[myTypeCounter] = new T;
		myTypes[myTypeCounter]->myID = myTypeCounter;
		myTypes[myTypeCounter]->mySemanticName = typeid(T).name();
		myTypeCounter++;
	}
	static CNodeType* myTypes[128];
	static unsigned short myTypeCounter;
	static unsigned short myTypeCount;
};

template<class T>
inline void CNodeType::SetPinType(unsigned int aPinIndex)
{
	if (aPinIndex < myPins.size())
	{
		myPins[aPinIndex].myData = T();
	}
}
