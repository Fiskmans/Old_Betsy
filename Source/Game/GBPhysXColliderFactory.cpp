#include "pch.h"
#include "GBPhysXColliderFactory.h"
#include "DataStructs.h"

GBPhysXColliderFactory::GBPhysXColliderFactory() :
	myGBPhysX(nullptr)
{
}

GBPhysXColliderFactory::~GBPhysXColliderFactory()
{
	myGBPhysX = nullptr;
	UnSubscribeToMessage(MessageType::SpawnGBPhysXBox);
}

void GBPhysXColliderFactory::Init(GBPhysX* aGBPhysXPtr)
{
	myGBPhysX = aGBPhysXPtr;
	SubscribeToMessage(MessageType::SpawnGBPhysXBox);
}

void GBPhysXColliderFactory::CreateGBPhysXCollider(CommonUtilities::Vector3<float> aPos, CommonUtilities::Vector3<float> aRot, CommonUtilities::Vector3<float> aScale)
{
	myGBPhysX->GBCreatePlayerBlockBox(aPos, aScale, aRot);
}

void GBPhysXColliderFactory::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::SpawnGBPhysXBox)
	{
		PhysXBoxInstanceBuffer* buffer = (PhysXBoxInstanceBuffer*)aMessage.myData;
		CommonUtilities::Vector3<float> pos = { buffer->position[0],buffer->position[1], buffer->position[2] };
		CommonUtilities::Vector3<float> rot = { buffer->rotation[0],buffer->rotation[1], buffer->rotation[2] };
		CommonUtilities::Vector3<float> scale = { buffer->scale[0],buffer->scale[1], buffer->scale[2] };
		CreateGBPhysXCollider(pos, rot, scale);
	}
}
