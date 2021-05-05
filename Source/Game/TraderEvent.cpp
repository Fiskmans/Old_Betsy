#include "pch.h"
#include "TraderEvent.h"

TraderEvent::TraderEvent()
{
}

TraderEvent::~TraderEvent()
{
}

void TraderEvent::Init()
{
}

void TraderEvent::Update(float aDeltaTime)
{
}

void TraderEvent::StartEvent()
{
	PostMaster::GetInstance()->SendMessages(MessageType::RespawnTrader);
}

void TraderEvent::IdleEvent()
{
}

void TraderEvent::EndEvent()
{
}

void TraderEvent::CleanUpEvent()
{
}
