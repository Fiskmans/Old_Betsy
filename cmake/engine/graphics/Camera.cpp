#include "engine/graphics/Camera.h"
#include "engine/graphics/RenderScene.h"

#include "engine/graphics/WindowManager.h"

#include "common/Macros.h"

namespace engine
{
	const tools::V2ui Camera::AdaptToScreen = tools::V2ui(0, 0);

	Camera::Camera(RenderScene& aScene, float aNearPlane, float aFarPlane, tools::V2ui aResolution)
		: myTransform(tools::M44f::Identity())
		, myNearPlane(aNearPlane)
		, myFarPlane(aFarPlane)
	{
		if (aResolution == AdaptToScreen)
		{
			myResolution = WindowManager::GetInstance().GetSize();
			myResolutionEvent = WindowManager::GetInstance().ResolutionChanged.Register(std::bind(&Camera::ChangeResolution, this, std::placeholders::_1));
		}
		else
		{
			myResolution = aResolution;
		}

		myScene = &aScene;
	}

	void Camera::ChangeResolution(tools::V2ui aResolution)
	{
		myResolution = aResolution;
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

		myTransform = tools::M44f::CreateRotationAroundPointX(aRotation[0], { position[0], position[1], position[2], 1 });
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

	std::vector<ModelInstance*> Camera::Cull() const
	{
		return myScene->CullByFrustum(GenerateFrustum());
	}

	void Camera::SetResolution(tools::V2ui aResolution)
	{
	}

	PerspectiveCamera::PerspectiveCamera(RenderScene& aScene, tools::Distance aNearPlane, tools::Distance aFarPlane, tools::Rotation aFOV, tools::V2ui aResolution)
		: Camera(aScene, aNearPlane, aFarPlane, aResolution)
	{
		myProjection = tools::M44f::Identity();
		myProjection.Row(2)[2] = aFarPlane / (aFarPlane - aNearPlane);
		myProjection.Row(3)[2] = (-aFarPlane * aNearPlane) / (aFarPlane - aNearPlane);
		myProjection.Row(3)[3] = 0;
		myProjection.Row(2)[3] = 1;

		SetFOV(aFOV);
	}

	void PerspectiveCamera::SetFOV(tools::Rotation aFOV)
	{
		myXFOV = aFOV / 2.f;
		if (myXFOV > ourMaxFOV)
		{
			myXFOV = ourMaxFOV;
		}
		UpdateYFOV();

		myProjection.Row(0)[0] = 1.f / tan(myXFOV);
	}

	void PerspectiveCamera::UpdateYFOV()
	{
		myYFOV = myXFOV * (static_cast<float>(myResolution[0]) / static_cast<float>(myResolution[1]));
		if (myYFOV > ourMaxFOV)
		{
			myYFOV = ourMaxFOV;
		}
		myProjection.Row(1)[1] = 1.f / tan(myYFOV);
	}

	tools::Frustum<float> PerspectiveCamera::GenerateFrustum() const
	{
		tools::Frustum<float> frustum;

		frustum.myRight = tools::Plane<float>(GetPosition(), tools::V4f(1.f, 0.f, 0.f, 0.f) * tools::M44f::CreateRotationAroundY(myXFOV * 0.5f) * myTransform);
		frustum.myLeft = tools::Plane<float>(GetPosition(), tools::V4f(-1.f, 0.f, 0.f, 0.f) * tools::M44f::CreateRotationAroundY(myXFOV * -0.5f) * myTransform);

		frustum.myTop = tools::Plane<float>(GetPosition(), tools::V4f(0.f, 1.f, 0.f, 0.f) * tools::M44f::CreateRotationAroundX(myYFOV * -0.5f) * myTransform);
		frustum.myBottom = tools::Plane<float>(GetPosition(), tools::V4f(0.f, -1.f, 0.f, 0.f) * tools::M44f::CreateRotationAroundX(myYFOV * 0.5f) * myTransform);

		tools::M33f rotation = myTransform.LowSubMatrix();

		frustum.myBack = tools::Plane<float>(tools::V3f(0, 0, myFarPlane) * rotation + GetPosition(), tools::V3f(0.f, 0.f, 1.f) * rotation);
		frustum.myFront = tools::Plane<float>(tools::V3f(0, 0, myNearPlane) * rotation + GetPosition(), tools::V3f(0.f, 0.f, -1.f) * rotation);

		return frustum;
	}

	void PerspectiveCamera::ChangeResolution(tools::V2ui aResolution)
	{
		Camera::ChangeResolution(aResolution);
		UpdateYFOV();
	}

	OrthogonalCamera::OrthogonalCamera(RenderScene& aScene, tools::V3f aSize, tools::V2ui aResolution)
		: Camera(aScene, 0, aSize[2], aResolution)
	{
		myProjection = tools::M44f::Identity();
		myProjection.Row(1)[1] = 2.f / aSize[0];
		myProjection.Row(2)[2] = 2.f / aSize[1];
		myProjection.Row(3)[3] = 1.f / aSize[2];
		myProjection.Row(4)[4] = 1.f;
	}

	tools::Frustum<float> OrthogonalCamera::GenerateFrustum() const
	{
		tools::Frustum<float> frustum;

		tools::V3f forward = GetForward();
		tools::V3f right = GetRight();
		tools::V3f up = GetUp();
		tools::V3f center = GetPosition() + (forward * myOrthoBounds[2] / 2.f);

		frustum.myLeft = tools::Plane<float>(center - right * myOrthoBounds[0] / 2.0f, -right);
		frustum.myRight = tools::Plane<float>(center + right * myOrthoBounds[0] / 2.0f, right);

		frustum.myBottom = tools::Plane<float>(center - up * myOrthoBounds[1] / 2.0f, -up);
		frustum.myTop = tools::Plane<float>(center + up * myOrthoBounds[1] / 2.0f, up);


		frustum.myBack = tools::Plane<float>(center - forward * myOrthoBounds[2] / 2.0f, -forward);
		frustum.myFront = tools::Plane<float>(center + forward * myOrthoBounds[2] / 2.0f, forward);

		return frustum;
	}

}
