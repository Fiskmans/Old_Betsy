#include "pch.h"
#include "Input.h"
#include "RenderScene.h"
#include "Camera.h"

Input::Input() : Observer({
		MessageType::InputMouseMoved
	})
{
}

V2f Input::MousePos()
{
	return myLastMousePos;
}

FRay Input::MouseRay()
{
	return RenderScene::GetInstance().GetMainCamera()->GetRay(MousePos());
}

void Input::RecieveMessage(const Message& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case MessageType::InputMouseMoved:
		myLastMousePos = *reinterpret_cast<const V2f*>(aMessage.myData);
		break;
	default:
		break;
	}
}
