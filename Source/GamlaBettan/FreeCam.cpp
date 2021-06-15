#include "pch.h"
#include "FreeCam.h"
#include "GamlaBettan\Scene.h"
#include "GamlaBettan\Camera.h"

FreeCam::FreeCam()
{
}

FreeCam::FreeCam(const UseDefaults&)
{
}

void FreeCam::Update(const FrameData& aFrameData, EntityID aEntityID)
{
	if (aFrameData.myInputHandler.IsKeyDown(CommonUtilities::InputHandler::Key::Key_Alt))
	{
		Camera* mainCam = Scene::GetInstance().GetMainCamera();

		static Point lastmp = aFrameData.myInputHandler.GetMousePosition();
		Point mp = aFrameData.myInputHandler.GetMousePosition();
		if (aFrameData.myInputHandler.IsMouseDown(CommonUtilities::InputHandler::Mouse::Mouse_Right))
		{
			const static float sensativity = 0.001f;

			float totalDiff = 1 / (1 + (mp.x - lastmp.x) * sensativity) * 1 / (1 + (mp.y - lastmp.y) * sensativity);
			mainCam->SetPosition(mainCam->GetPosition() * totalDiff);
		}
		else if (aFrameData.myInputHandler.IsMouseDown(CommonUtilities::InputHandler::Mouse::Mouse_Left))
		{
			V3F pos = mainCam->GetPosition();
			float length = pos.Length();
			pos += mainCam->GetRight() * length * -static_cast<float>(mp.x - lastmp.x) * 0.001f;
			pos += mainCam->GetUp() * length * static_cast<float>(mp.y - lastmp.y) * 0.001f;
			pos = pos.GetNormalized() * length;

			mainCam->SetPosition(pos);
			mainCam->LookAt(V3F(0, 0, 0));
		}
		else
		{
			lastmp = aFrameData.myInputHandler.GetMousePosition();
		}
		lastmp = mp;
	}
	else
	{
		CommonUtilities::Vector3<float> movement = { 0.f, 0.f, 0.f };
		CommonUtilities::Vector3<float> rotation = { 0.f, 0.f, 0.f };
		if (aFrameData.myInputHandler.IsKeyDown(CommonUtilities::InputHandler::Key::Key_W))
		{
			movement.z += myMovementSpeed * aFrameData.myDeltaTime;
		}
		if (aFrameData.myInputHandler.IsKeyDown(CommonUtilities::InputHandler::Key::Key_S))
		{
			movement.z -= myMovementSpeed * aFrameData.myDeltaTime;
		}

		if (aFrameData.myInputHandler.IsKeyDown(CommonUtilities::InputHandler::Key::Key_D))
		{
			movement.x += myMovementSpeed * aFrameData.myDeltaTime;
		}
		if (aFrameData.myInputHandler.IsKeyDown(CommonUtilities::InputHandler::Key::Key_A))
		{
			movement.x -= myMovementSpeed * aFrameData.myDeltaTime;
		}

		if (aFrameData.myInputHandler.IsKeyDown(CommonUtilities::InputHandler::Key::Key_Space))
		{
			movement.y += myMovementSpeed * aFrameData.myDeltaTime;
		}
		if (aFrameData.myInputHandler.IsKeyDown(CommonUtilities::InputHandler::Key::Key_Shift))
		{
			movement.y -= myMovementSpeed * aFrameData.myDeltaTime;
		}

		if (aFrameData.myInputHandler.IsKeyDown(CommonUtilities::InputHandler::Key::Key_Q))
		{
			rotation.y -= myRotationSpeed * aFrameData.myDeltaTime;
		}
		if (aFrameData.myInputHandler.IsKeyDown(CommonUtilities::InputHandler::Key::Key_E))
		{
			rotation.y += myRotationSpeed * aFrameData.myDeltaTime;
		}
		if (aFrameData.myInputHandler.IsKeyDown(CommonUtilities::InputHandler::Key::Key_Z))
		{
			rotation.x -= myRotationSpeed * aFrameData.myDeltaTime;
		}
		if (aFrameData.myInputHandler.IsKeyDown(CommonUtilities::InputHandler::Key::Key_X))
		{
			rotation.x += myRotationSpeed * aFrameData.myDeltaTime;
		}
		Camera* mainCam = Scene::GetInstance().GetMainCamera();

		mainCam->Move(movement);
		mainCam->Rotate(rotation);
	}
}

#if USEIMGUI
void FreeCam::ImGui(EntityID aEntityID)
{
}
#endif
