#ifndef ENGINE_GRAPHICS_CAMERA_H
#define ENGINE_GRAPHICS_CAMERA_H

#include "engine/assets/Asset.h"
#include "engine/assets/ModelInstance.h"

#include "tools/MathVector.h"
#include "tools/Matrix4x4.h"
#include "tools/Matrix3x3.h"
#include "tools/Literals.h"
#include "tools/Frustum.h"
#include "tools/Event.h"
#include "tools/Ray.h"

namespace engine
{
	class RenderScene;

	class Camera
	{
	public:

		const static tools::V2ui AdaptToScreen;
	
		Camera(RenderScene& aScene, float aNearPlane, float aFarPlane, tools::V2ui aResolution);
		virtual ~Camera() = default;

		virtual void ChangeResolution(tools::V2ui aResolution);

		void SetTransform(tools::V4f aPosition, tools::V3f aRotation);
		void SetTransform(tools::M44f aTransform);
		void SetRotation(tools::V3f aRotation);
		void SetRotation(tools::M33f aRotationMatrix);
		void SetPosition(tools::V4f aPosition);
		void Move(tools::V3f aMovement);
		void Rotate(tools::V3f aRotation);
		void RotateWorldSpace(tools::V3f aRotation);
	
		template<class T>
		void LookAt(T* aTarget);
		void LookAt(tools::V3f aTarget);

		tools::FRay GetRay(tools::V2f aPosition);

		tools::M44f GetTransform() const;
		tools::M44f GetProjection() const;

		tools::V3f GetForward() const;
		tools::V3f GetFlatForward() const;
		tools::V3f GetUp() const;
		tools::V3f GetRight() const;

		tools::V3f GetPosition() const;

		RenderScene& GetScene() { return *myScene; }

		virtual tools::Frustum<float> GenerateFrustum() const = 0;

		std::vector<ModelInstance*> Cull() const;

	private:
		void SetResolution(tools::V2ui aResolution);

	protected:
		tools::V2ui myResolution;
		RenderScene* myScene;
		
		tools::M44f myTransform;
		tools::M44f myProjection;

		fisk::tools::EventReg myResolutionEvent;

		float myNearPlane;
		float myFarPlane;
	};

	class PerspectiveCamera final : public Camera
	{
	public:
		PerspectiveCamera(RenderScene& aScene, tools::Distance aNearPlane, tools::Distance aFarPlane, tools::Rotation aFOV, tools::V2ui aResolution = AdaptToScreen);

		void SetFOV(tools::Rotation aFOV);

		void UpdateYFOV();

		tools::Frustum<float> GenerateFrustum() const override;

	protected:
		static constexpr float ourMaxFOV = (PI_F / 2.f) * (179.f / 180.f);

		void ChangeResolution(tools::V2ui aResolution) override;
		float myXFOV;
		float myYFOV;
	};

	class OrthogonalCamera final : public Camera
	{
	public:
		OrthogonalCamera(RenderScene& aScene, tools::V3f aSize, tools::V2ui aResolution = AdaptToScreen);

		tools::Frustum<float> GenerateFrustum() const override;

	private:
		tools::V3f myOrthoBounds;
	};

	template<class T>
	inline void Camera::LookAt(T* aTarget)
	{
		LookAt(aTarget->GetPosition());
	}

}

#endif