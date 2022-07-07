#ifndef ENGINE_GRAPHICS_CAMERA_H
#define ENGINE_GRAPHICS_CAMERA_H

#include "engine/graph/Graph.h"

#include "tools/MathVector.h"
#include "tools/Matrix4x4.h"
#include "tools/Matrix3x3.h"
#include "tools/Literals.h"
#include "tools/PlaneVolume.h"
#include "tools/Event.h"
#include "tools/Ray.h"

namespace engine
{
	class Camera
	{
	public:

		const static tools::V2ui AdaptToScreen;
	
		Camera(float aNearPlane, float aFarPlane, tools::V2ui aResolution);
		virtual ~Camera();


		virtual void OnResolutionChanged(tools::V2ui aResolution);

		bool Init(tools::V3f aBoundingBox);
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

		virtual tools::PlaneVolume<float> GenerateFrustum() const = 0;

	protected:
	
		engine::graph::Graph myRenderGraph;
		tools::M44f myTransform;
		tools::M44f myProjection;

		tools::EventID myResolutionEvent = tools::NullEventId;
		tools::V2ui myResolution;

		float myNearPlane;
		float myFarPlane;
	};

	class PerspectiveCamera final : public Camera
	{
	public:
		PerspectiveCamera(float aNearPlane, float aFarPlane, float aFOV, tools::V2ui aResolution = AdaptToScreen);

		void SetFOVRad(float aFOV);

		tools::PlaneVolume<float> GenerateFrustum() const override;

	protected:
		void OnResolutionChanged(tools::V2ui aResolution) override;
		float myXFOV;
		float myYFOV;
	};

	class OrthogonalCamera final : public Camera
	{
	public:
		OrthogonalCamera(tools::V3f aSize, tools::V2ui aResolution = AdaptToScreen);

		tools::PlaneVolume<float> GenerateFrustum() const override;

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