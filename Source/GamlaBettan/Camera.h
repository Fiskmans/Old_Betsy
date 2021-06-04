#pragma once

#include "Tools/Observer.hpp"
#include "CommonUtilities/PlaneVolume.hpp"

namespace CommonUtilities
{
	template<class T>
	class Vector3;
}

class Camera : public Observer
{
public:
	enum class CameraType
	{
		Orthographic
	};

	Camera();
	~Camera();

	bool Init(float aFoV, V2ui aResolution, float aNear, float aFar, const bool aShouldAdaptToWindowRes);
	bool Init(CommonUtilities::Vector3<float> aBoundingBox);
	void SetTransform(CommonUtilities::Vector3<float> aPosition, CommonUtilities::Vector3<float> aRotation);
	void SetTransform(M44f aTransform);
	void SetRotation(CommonUtilities::Vector3<float> aRotation);
	void SetRotation(CommonUtilities::Matrix3x3<float> aRotationMatrix);
	void SetPosition(CommonUtilities::Vector3<float> aPosition);
	void Move(CommonUtilities::Vector3<float> aMovement);
	void Rotate(CommonUtilities::Vector3<float> aRotation);
	void RotateWorldSpace(CommonUtilities::Vector3<float> aRotation);
	void SetFov(const float aFov);
	void SetResolution(const V2ui& aResolution);
	float GetFoV() const;
	void RecieveMessage(const Message& aMessage) override;
	
	template<class T>
	void LookAt(T* aTarget);
	void LookAt(CommonUtilities::Vector3<float> aTarget);


	CommonUtilities::Matrix4x4<float> GetTransform() const;
	CommonUtilities::Matrix4x4<float> GetProjection() const;

	CommonUtilities::Vector3<float> GetForward() const;
	CommonUtilities::Vector3<float> GetFlatForward() const;
	CommonUtilities::Vector3<float> GetUp() const;
	CommonUtilities::Vector3<float> GetRight() const;

	CommonUtilities::Vector3<float> GetPosition() const;

	CommonUtilities::PlaneVolume<float> GenerateFrustum() const;

private:
	CommonUtilities::Matrix4x4<float> myTransform;
	CommonUtilities::Matrix4x4<float> myProjection;
	CommonUtilities::Matrix4x4<float> myProjection2;

	CommonUtilities::Vector3<float> myOrthoBounds;
	V2ui myResolution;
	float myNearPlane;
	float myFarPlane;
	float myFOV;
	bool myIsListening;
	bool myIsOrthogonal;

};

template<class T>
inline void Camera::LookAt(T* aTarget)
{
	LookAt(aTarget->GetPosition());
}
