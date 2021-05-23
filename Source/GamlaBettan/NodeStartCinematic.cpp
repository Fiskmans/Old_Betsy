#include "pch.h"
#include "NodeStartCinematic.h"
#include <iostream>
#include "CNodeInstance.h"
#include "NodeDataTypes.h"
#include "../Game/CinematicState.h"
#include <PostMaster.hpp>

NodeStartCinematic::NodeStartCinematic()
{
	myPins.push_back(CPin("In", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Flow));
	myPins.push_back(CPin("Out", CPin::PinTypeInOut::PinTypeInOut_OUT, CPin::PinType::Flow));
	myPins.push_back(CPin("Cinematic", CPin::PinTypeInOut::PinTypeInOut_IN, CPin::PinType::Data));
	SetPinType<std::string>(2);
}

int NodeStartCinematic::OnEnter(CNodeInstance* aTriggeringNodeInstance, const int aEnteredViae)
{
	std::string name;
	if (!aTriggeringNodeInstance->ReadData(2, name))
	{
		return -1;
	}

	class Scene* scene = aTriggeringNodeInstance->ourPollingStation->GetScene();
	class ParticleFactory* factory = aTriggeringNodeInstance->ourPollingStation->GetParticleFactory();
	class SpriteFactory* sfactory = aTriggeringNodeInstance->ourPollingStation->GetSpriteFactory();
	if (scene && factory && sfactory)
	{
		CinematicState* state = new CinematicState();
		if (state->Init("Data/Cinematics/" + name + ".cinm", scene, factory, sfactory))
		{
			Message message;
			message.myMessageType = MessageType::PushState;
			state->SetMain(false);
			message.myData = state;
			PostMaster::GetInstance()->SendMessages(message);
		}
		else
		{
			delete state;
		}
	}

	return 1;
}
