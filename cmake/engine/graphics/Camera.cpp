#include "engine/graphics/Camera.h"

#include "engine/graphics/WindowManager.h"

#include "common/Macros.h"

#include "engine/graph/NodeManager.h"
#include "engine/graph/nodes/RenderMergeNode.h"

namespace engine
{
	const tools::V2ui Camera::AdaptToScreen = tools::V2ui(0, 0);

	Camera::Camera(float aNearPlane, float aFarPlane, tools::V2ui aResolution)
		: myRenderGraph("Render graph", { &myRenderTexture }, { &myResolutionExport })
		, myTransform(tools::M44f::Identity())
		, myNearPlane(aNearPlane)
		, myFarPlane(aFarPlane)
	{
		myRenderGraph.AddNode<graph::node::RenderMergeNode>(ImVec2(0, 0));

		if (aResolution == AdaptToScreen)
		{
			myResolutionExport.Write(WindowManager::GetInstance().GetSize());
			myResolutionEvent = WindowManager::GetInstance().ResolutionChanged.Register(std::bind(&Camera::OnResolutionChanged, this, std::placeholders::_1));
		}
		else
		{
			myResolutionExport.Write(aResolution);
		}


	}

	Camera::~Camera()
	{
		if (myResolutionEvent != tools::NullEventId)
		{
			WindowManager::GetInstance().ResolutionChanged.UnRegister(myResolutionEvent);
			myResolutionEvent = tools::NullEventId;
		}
	}

	void Camera::OnResolutionChanged(tools::V2ui aResolution)
	{
		// TODO Resize textures
	}

	void Camera::SetTransform(tools::V4f aPosition, tools::V3f aRotation)
	{
		SetPosition(aPosition);
		SetRotation(aRotation);
	}


	void Camera::SetTransform(tools::M44f aTransform)
	{
		myTransform = aTransform;
	}

	void Camera::SetRotation(tools::V3f aRotation)
	{
		tools::V4f position = myTransform.Row(3);

		myTransform  = tools::M44f::CreateRotationAroundPointX(aRotation[0], { position[0], position[1], position[2], 1 });
		myTransform *= tools::M44f::CreateRotationAroundPointY(aRotation[1], { position[0], position[1], position[2], 1 });
		myTransform *= tools::M44f::CreateRotationAroundPointZ(aRotation[2], { position[0], position[1], position[2], 1 });
		
		myTransform.Row(3) = position;
	}

	void Camera::SetRotation(tools::M33f aRotationMatrix)
	{
		for (size_t y = 0; y < 3; y++)
		{
			for (size_t x = 0; x < 3; x++)
			{
				myTransform.Row(y)[x] = aRotationMatrix.Row(y)[x];
			}
		}
	}

	void Camera::SetPosition(tools::V4f aPosition)
	{
		myTransform.Row(4) = aPosition;
		myTransform.Row(4)[3] = 1.f;
	}

	void Camera::Move(tools::V3f aMovement)
	{
		tools::V4f aPosition = aMovement.Extend(1.f);
		aPosition = aPosition * myTransform;

		myTransform.Row(3) = aPosition;
	}

	void Camera::Rotate(tools::V3f aRotation)
	{
		tools::V4f position = myTransform.Row(3);

		tools::M44f mat = tools::M44f::CreateRotationAroundX(aRotation[0]) * tools::M44f::CreateRotationAroundY(aRotation[1]) * tools::M44f::CreateRotationAroundZ(aRotation[2]);

		mat *= myTransform;
		myTransform = mat;

		myTransform.Row(3) = position;
	}

	void Camera::RotateWorldSpace(tools::V3f aRotation)
	{
		tools::V4f position = myTransform.Row(3);

		tools::M44f mat = tools::M44f::CreateRotationAroundX(aRotation[0]) * tools::M44f::CreateRotationAroundY(aRotation[1]) * tools::M44f::CreateRotationAroundZ(aRotation[2]);

		myTransform *= mat;
		mat = myTransform;

		myTransform.Row(3) = position;
	}

	void Camera::LookAt(tools::V3f aTarget)
	{
		const tools::V3f worldUp(0, 1, 0);

		tools::V3f pos(myTransform.Row(3));
		tools::V3f forward = aTarget - pos;

		tools::V3f right = forward.Cross(-worldUp);
		tools::V3f up = -right.Cross(forward);

		if (right.LengthSqr() < 0.5f)
		{
			right = tools::V3f(0, 0, 1);
			up = tools::V3f(1, 0, 0);
		}

		up.Normalize();
		right.Normalize();
		forward.Normalize();

		myTransform.Row(0) = right.Extend(0.f);
		myTransform.Row(1) = up.Extend(0.f);
		myTransform.Row(2) = forward.Extend(0.f);
		myTransform.Row(3) = pos.Extend(1.f);
	}

	tools::FRay Camera::GetRay(tools::V2f aPosition)
	{
		tools::V2f pos = aPosition * tools::V2f(2.f, -2.f) + tools::V2f(-1, 1);
		tools::V4f start = pos.Extend(0.f, 1.f);
		tools::V4f end = pos.Extend(1.f, 1.f);

		tools::M44f unProj = (myTransform.RealInverse() * myProjection).RealInverse();
		start = start * unProj;
		end = end * unProj;

		start /= start[3];
		end /= end[3];

		tools::V3f dir = end - start;
		return tools::FRay(start, dir.GetNormalized());
	}

	tools::M44f Camera::GetTransform() const
	{
		return myTransform;
	}

	tools::M44f Camera::GetProjection() const
	{
		return myProjection;
	}

	tools::V3f Camera::GetForward() const
	{
		tools::V4f forward(0, 0, 1, 0);
		forward = forward * myTransform;
		return forward;
	}
	tools::V3f Camera::GetFlatForward() const
	{
		tools::V4f forward(0, 0, 1, 0);
		forward = forward * myTransform;
		return tools::V3f(forward[0], 0.f, forward[2]).GetNormalized();
	}

	tools::V3f Camera::GetUp() const
	{
		tools::V4f up(0, 1, 0, 0);
		up = up * myTransform;
		return up;
	}
	tools::V3f Camera::GetRight() const
	{
		tools::V4f right(1, 0, 0, 0);
		right = right * myTransform;
		return right;
	}

	tools::V3f Camera::GetPosition() const
	{
		return myTransform.Row(3);
	}

	void Camera::SetResolution(tools::V2ui aResolution)
	{
		myResolutionExport.Write(aResolution);
	}

	PerspectiveCamera::PerspectiveCamera(float aNearPlane, float aFarPlane, float aFOV, tools::V2ui aResolution)
		: Camera(aNearPlane, aFarPlane, aResolution)
	{
		myProjection = tools::M44f::Identity();
		myProjection.Row(2)[2] = aFarPlane / (aFarPlane - aNearPlane);
		myProjection.Row(3)[2] = (-aFarPlane * aNearPlane) / (aFarPlane - aNearPlane);
		myProjection.Row(3)[3] = 0;
		myProjection.Row(2)[3] = 1;

		SetFOVRad(aFOV);
	}

	void PerspectiveCamera::SetFOVRad(float aFOV)
	{
		tools::V2ui resolution = myResolutionExport.Get();
		const float maxFOV = (PI_F / 2.f) * (179.f / 180.f);
		myXFOV = aFOV / 2.f;
		myYFOV = aFOV / 2.f * (static_cast<float>(resolution[0]) / static_cast<float>(resolution[1]));
		if (myXFOV > maxFOV)
		{
			myXFOV = maxFOV;
		}
		if (myYFOV > maxFOV)
		{
			myYFOV = maxFOV;
		}


		myProjection.Row(0)[0] = 1.f / tan(myXFOV);
		myProjection.Row(1)[1] = 1.f / tan(myYFOV);
	}

	tools::PlaneVolume<float> PerspectiveCamera::GenerateFrustum() const
	{
		tools::PlaneVolume<float> frustum;

		frustum.AddPlane(tools::Plane<float>(GetPosition(), tools::V4f(0.f, 0.f, 1.f, 0.f) * tools::M44f::CreateRotationAroundY((PI_F + myXFOV) * 0.5f) * myTransform));
		frustum.AddPlane(tools::Plane<float>(GetPosition(), tools::V4f(0.f, 0.f, 1.f, 0.f) * tools::M44f::CreateRotationAroundY((PI_F + myXFOV) * -0.5f) * myTransform));

		frustum.AddPlane(tools::Plane<float>(GetPosition(), tools::V4f(0.f, 0.f, 1.f, 0.f) * tools::M44f::CreateRotationAroundX((PI_F + myYFOV) * 0.5f) * myTransform));
		frustum.AddPlane(tools::Plane<float>(GetPosition(), tools::V4f(0.f, 0.f, 1.f, 0.f) * tools::M44f::CreateRotationAroundX((PI_F + myYFOV) * -0.5f) * myTransform));

		tools::M33f rotation = myTransform.LowSubMatrix();

		frustum.AddPlane(tools::Plane<float>(tools::V3f(0, 0, myFarPlane) * rotation + GetPosition(), tools::V3f(0.f, 0.f, 1.f) * rotation));
		frustum.AddPlane(tools::Plane<float>(tools::V3f(0, 0, myNearPlane) * rotation + GetPosition(), tools::V3f(0.f, 0.f, -1.f) * rotation));

		return frustum;
	}

	void PerspectiveCamera::OnResolutionChanged(tools::V2ui aResolution)
	{
		myProjection.Row(1)[1] = myProjection.Row(0)[1] * (static_cast<float>(aResolution[0]) / static_cast<float>(aResolution[1]));
	}

	OrthogonalCamera::OrthogonalCamera(tools::V3f aSize, tools::V2ui aResolution)
		: Camera(0, aSize[2], aResolution)
	{
		myProjection = tools::M44f::Identity();
		myProjection.Row(1)[1] = 2.f / aSize[0];
		myProjection.Row(2)[2] = 2.f / aSize[1];
		myProjection.Row(3)[3] = 1.f / aSize[2];
		myProjection.Row(4)[4] = 1.f;
	}

	tools::PlaneVolume<float> OrthogonalCamera::GenerateFrustum() const
	{
		tools::PlaneVolume<float> frustum;
		tools::V3f forward = GetForward();
		tools::V3f right = GetRight();
		tools::V3f up = GetUp();
		tools::V3f center = GetPosition() + (forward * myOrthoBounds[2] / 2.f);

		frustum.AddPlane(tools::Plane<float>(center - right * myOrthoBounds[0] / 2.0f, -right));
		frustum.AddPlane(tools::Plane<float>(center + right * myOrthoBounds[0] / 2.0f, right));

		frustum.AddPlane(tools::Plane<float>(center - up * myOrthoBounds[1] / 2.0f, -up));
		frustum.AddPlane(tools::Plane<float>(center + up * myOrthoBounds[1] / 2.0f, up));


		frustum.AddPlane(tools::Plane<float>(center - forward * myOrthoBounds[2] / 2.0f, -forward));
		frustum.AddPlane(tools::Plane<float>(center + forward * myOrthoBounds[2] / 2.0f, forward));
		return frustum;
	}

}
