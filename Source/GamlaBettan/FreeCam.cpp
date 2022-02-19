#include "pch.h"
#include "FreeCam.h"
#include "GamlaBettan\RenderScene.h"
#include "GamlaBettan\Camera.h"

FreeCam::FreeCam()
{
}

FreeCam::FreeCam(const FiskJSON::Object& aObject)
{
	aObject["rotSpeed"].GetIf(myRotationSpeed);
	aObject["movSpeed"].GetIf(myMovementSpeed);
}

FreeCam::FreeCam(const UseDefaults&)
{
}

void FreeCam::Update(const FrameData& aFrameData, EntityID /*aEntityID*/)
{
	if (aFrameData.myInputHandler.IsKeyDown(CommonUtilities::InputHandler::Key::Key_Alt))
	{
		Camera* mainCam = RenderScene::GetInstance().GetMainCamera();

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
}

#if USEIMGUI
void FreeCam::ImGui(EntityID /*aEntityID*/)
{
}

void FreeCam::Serialize(FiskJSON::Object& aObject)
{
	aObject.AddValueChild("rotSpeed", myRotationSpeed);
	aObject.AddValueChild("movSpeed", myMovementSpeed);
}
#endif
