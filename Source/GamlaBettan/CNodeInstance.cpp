#include "pch.h"
#include "CNodeInstance.h"
#include "CNodeType.h"
#include <assert.h>
#include "CGraphManager.h"


NodePollingStation* CNodeInstance::ourPollingStation;

CNodeInstance::CNodeInstance(bool aCreateNewUID)
	:myUID(aCreateNewUID)
{

	
}

bool IsOutput(std::vector<CPin>& pins, unsigned int aID)
{
	for (auto& pin : pins)
	{
		if (pin.myPinType == CPin::PinTypeInOut::PinTypeInOut_OUT && pin.myUID.AsInt() == aID)
		{
			return true;
		}
	}
	return false;
}

void CNodeInstance::Enter(const int aEnteredVia)
{
	int outputIndex = myNodeType->DoEnter(this, GetLocalPinID(aEnteredVia));
	outputIndex = outputIndex > -1 ? myPins[outputIndex].myUID.AsInt() : -1;
	if (outputIndex > -1)
	{
		for (auto& link : myLinks)
		{
			if ((int)link.myFromPinID == outputIndex && IsOutput(myPins, link.myFromPinID))
			{
				link.myLink->Enter(link.myToPinID);
			}
			else if (!IsOutput(myPins, link.myFromPinID) )
			{
				CGraphManager::ShowFlow(link.myLinkID);
			}
		}
	}
}

void CNodeInstance::ExitVia(unsigned int aPinIndex)
{
	if (myPins.size() <= aPinIndex)
	{
		ONETIMEWARNING("Node tied to exit via pin [" + std::to_string(aPinIndex) + "] when there was only [" + std::to_string(myPins.size()) + "]","");
		return;
	}
	CPin& pin = myPins[aPinIndex];
	std::vector< SNodeInstanceLink*> links = GetLinksFromPin(pin.myUID.AsInt());
	for (auto link : links)
	{
		link->myLink->Enter(link->myToPinID);
	}
}

void CNodeInstance::ConstructUniquePins()
{
	myPins = myNodeType->GetPins();
}

bool CNodeInstance::CanAddLink(unsigned int aPinIdFromMe)
{
	CPin* pin = GetPinFromID(aPinIdFromMe);

	if (pin->myPinType == CPin::PinTypeInOut::PinTypeInOut_IN &&  pin->myVariableType != CPin::PinType::Flow)
	{
		if (GetLinksFromPin(aPinIdFromMe).size() != 0)
		{
			return false;
		}
	}
	return true;
}

bool CNodeInstance::AddLinkToVia(CNodeInstance* aLink, unsigned int aPinIdFromMe, unsigned int aPinIdToMe, unsigned int aLinkID)
{
	CPin* pin = GetPinFromID(aPinIdFromMe);

	if (pin->myPinType == CPin::PinTypeInOut::PinTypeInOut_IN &&  pin->myVariableType != CPin::PinType::Flow)
	{
		if (GetLinksFromPin(aPinIdFromMe).size() != 0)
		{
			return false;
		}
	}
	

	myLinks.push_back(SNodeInstanceLink(aLink, aPinIdFromMe, aPinIdToMe, aLinkID));
	return true;
}

void CNodeInstance::RemoveLinkToVia(CNodeInstance* aLink, unsigned int aPinThatIOwn)
{
	for (int i=0; i< myLinks.size(); i++)
	{
		if (myLinks[i].myLink == aLink)
		{
			if (myLinks[i].myFromPinID == aPinThatIOwn)
			{
				myLinks.erase(myLinks.begin() + i);
				return;
			}
			else if (myLinks[i].myToPinID == aPinThatIOwn)
			{
				myLinks.erase(myLinks.begin() + i);
				return;
			}
		}
	}
	assert(0);
}


bool CNodeInstance::IsPinConnected(CPin& aPin)
{
	return GetLinksFromPin(aPin.myUID.AsInt()).size() > 0;
}

std::string CNodeInstance::GetNodeName()
{
	return myNodeType->GetNodeName();
}

void CNodeInstance::ChangePinTypes(CPin::PinType aType)
{
	for (auto& pin : myPins)
	{
		pin.myVariableType = aType;
	}
}

int CNodeInstance::GetLocalPinID(const int anID)
{
	for (size_t i = 0; i < myPins.size(); i++)
	{
		if (myPins[i].myUID.AsInt() == anID)
		{
			return i;
		}
	}

	return -1;
}

std::any CNodeInstance::ReadRaw(unsigned int aPinIndex)
{// If we dont have any data, but or link might have it, the link pin might have data written to it as well, then return that
	if (!myNodeType->IsFlowNode())
	{
		if (myPins[aPinIndex].myPinType == CPin::PinTypeInOut::PinTypeInOut_IN)
		{
			std::vector< SNodeInstanceLink*> links = GetLinksFromPin(myPins[aPinIndex].myUID.AsInt());
			if (links.size() > 0)
			{
				// Get data from first link, wierd if we have more than two links to fetch data from
				int pinIndex = links[0]->myLink->GetPinIndexFromPinUID(links[0]->myToPinID);
				if (pinIndex == -1)
				{
					assert(0);
				}

				CGraphManager::ShowFlow(links[0]->myLinkID);
				//we have a link in a node that is supposed only to store data, apparently this is connected aswell
				return links[0]->myLink->ReadRaw(pinIndex);
			}
		}
		else
		{
			Enter(-1);
		}
	}
	else
	{
		if (myPins[aPinIndex].myPinType == CPin::PinTypeInOut::PinTypeInOut_IN)
		{
			std::vector< SNodeInstanceLink*> links = GetLinksFromPin(myPins[aPinIndex].myUID.AsInt());
			if (links.size() > 0)
			{
				int indexInVector = -1;
				for (int i = 0; i < links[0]->myLink->myPins.size(); i++)
				{
					indexInVector = i;
					if (links[0]->myLink->myPins[i].myUID.AsInt() == links[0]->myToPinID) // links[0] always get the data from the first connection, more connections are illigal
					{
						break;
					}
				}

				assert(indexInVector != -1);
				return links[0]->myLink->ReadRaw(indexInVector);
			}
		}
	}

	if (aPinIndex < 0 || aPinIndex >= myPins.size())
	{
		return std::any();
	}

	CPin& dataPin = myPins[aPinIndex];
	return dataPin.myData;
}

std::vector< SNodeInstanceLink*> CNodeInstance::GetLinksFromPin(unsigned int aPinToFetchFrom)
{
	std::vector< SNodeInstanceLink*> links;
	for (int i = 0; i < myLinks.size(); i++)
	{
		if (myLinks[i].myFromPinID == aPinToFetchFrom)
		{
			links.push_back(&myLinks[i]);
		}
		else if (myLinks[i].myToPinID == aPinToFetchFrom)
		{
			links.push_back(&myLinks[i]);
		}
	}
	return links;

}

CPin* CNodeInstance::GetPinFromID(unsigned int aUID)
{
	for (auto& pin : myPins)
	{
		if (pin.myUID.AsInt() == aUID)
		{
			return &pin;
		}
	}
	return nullptr;
}

int CNodeInstance::GetPinIndexFromPinUID(unsigned int aPinUID)
{
	for (int i = 0; i < myPins.size(); i++)
	{
		if (myPins[i].myUID.AsInt() == aPinUID)
		{
			return i;
		}
	}
	return -1;
}


void CNodeInstance::DebugUpdate()
{
	if (myNodeType)
	{
		myNodeType->DebugUpdate(this);
	}
}

void CNodeInstance::VisualUpdate(float aTimeDelta)
{
	myEnteredTimer -= aTimeDelta;
	if (myEnteredTimer <= 0.0f)
	{
		myEnteredTimer = 0.0f;
	}
}

