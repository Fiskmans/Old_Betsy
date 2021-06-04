#include "pch.h"
#include "Camera.h"
#include "CommonUtilities\PlaneVolume.hpp"

Camera::Camera() :
	myFarPlane(0.f),
	myNearPlane(0.f),
	myFOV(0.f),
	myIsListening(false),
	myIsOrthogonal(false),
	Observer({
			MessageType::WindowResize
		})
{

}
Camera::~Camera()
{
}

bool Camera::Init(float aFoV, V2ui aResolution, float aNear, float aFar, const bool aShouldAdaptToWindowRes)
{
	myFarPlane = aFar;
	myNearPlane = aNear;
	myFOV = aFoV;
	myProjection = CommonUtilities::Matrix4x4<float>::Identity();
	SetFov(aFoV);
	SetResolution(aResolution);
	myProjection(3, 3) = aFar / (aFar - aNear);
	myProjection(4, 3) = (-aFar * aNear) / (aFar - aNear);
	myProjection(4, 4) = 0;
	myProjection(3, 4) = 1;


	myProjection2(3, 3) = aFar / (aFar - aNear);
	myProjection2(4, 3) = (-aFar * aNear) / (aFar - aNear);
	myProjection2(4, 4) = 0;
	myProjection2(3, 4) = 1;

	if (aShouldAdaptToWindowRes)
	{
		myIsListening = true;
	}

	return true;
}

bool Camera::Init(CommonUtilities::Vector3<float> aBoundingBox)
{
	myFarPlane = aBoundingBox.z;
	myNearPlane = 0;
	myProjection = CommonUtilities::Matrix4x4<float>::Identity();
	myProjection(1, 1) = 2.f / aBoundingBox.x;
	myProjection(2, 2) = 2.f / aBoundingBox.y;
	myProjection(3, 3) = 1.f / aBoundingBox.z;
	myProjection(4, 4) = 1.f;
	myIsOrthogonal = true;
	myOrthoBounds = aBoundingBox;

	return true;
}

void Camera::SetFov(const float aFov)
{
	float calculatedFoV = 1.f / (tan((aFov / 2) * (PI / 180.f)));

	myProjection(1, 1) = calculatedFoV;
	myProjection(2, 2) = calculatedFoV * (static_cast<float>(myResolution.x) / static_cast<float>(myResolution.y));
	myFOV = aFov;
}

float Camera::GetFoV() const
{
	return myFOV;
}

void Camera::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::WindowResize && myIsListening)
	{
		SetResolution(*reinterpret_cast<const V2ui*>(aMessage.myData));
	}
}

void Camera::SetResolution(const V2ui& aResolution)
{
	myProjection(2, 2) = myProjection(1, 1) * (static_cast<float>(aResolution.x) / static_cast<float>(aResolution.y));
	myResolution = aResolution;
}

void Camera::SetTransform(CommonUtilities::Vector3<float> aPosition, CommonUtilities::Vector3<float> aRotation)
{
	SetPosition(aPosition);
	SetRotation(aRotation);
}

void Camera::SetTransform(M44f aTransform)
{
	myTransform = aTransform;
}

void Camera::SetRotation(CommonUtilities::Vector3<float> aRotation)
{
	CommonUtilities::Vector3<float> position = { myTransform(4,1),myTransform(4,2),myTransform(4,3) };

	myTransform = CU::Matrix4x4<float>::CreateRotationAroundPointX(aRotation.x, { position.x, position.y, position.z, 1 });
	myTransform *= CU::Matrix4x4<float>::CreateRotationAroundPointY(aRotation.y, { position.x, position.y, position.z, 1 });
	myTransform *= CU::Matrix4x4<float>::CreateRotationAroundPointZ(aRotation.z, { position.x, position.y, position.z, 1 });

	//myTransform.RotateAroundPointX(aRotation.x, { position.x, position.y, position.z, 1 }); //Matrices multiplied in diffrerent order from above??? Not sure which is correct
	//myTransform.RotateAroundPointY(aRotation.y, { position.x, position.y, position.z, 1 });
	//myTransform.RotateAroundPointZ(aRotation.z, { position.x, position.y, position.z, 1 });

	myTransform(4, 1) = position.x;
	myTransform(4, 2) = position.y;
	myTransform(4, 3) = position.z;
}

void Camera::SetRotation(CommonUtilities::Matrix3x3<float> aRotationMatrix)
{
	myTransform(1, 1) = aRotationMatrix(1, 1);
	myTransform(1, 2) = aRotationMatrix(1, 2);
	myTransform(1, 3) = aRotationMatrix(1, 3);
	myTransform(2, 1) = aRotationMatrix(2, 1);
	myTransform(2, 2) = aRotationMatrix(2, 2);
	myTransform(2, 3) = aRotationMatrix(2, 3);
	myTransform(3, 1) = aRotationMatrix(3, 1);
	myTransform(3, 2) = aRotationMatrix(3, 2);
	myTransform(3, 3) = aRotationMatrix(3, 3);
}

void Camera::SetPosition(CommonUtilities::Vector3<float> aPosition)
{
	myTransform(4, 1) = aPosition.x;
	myTransform(4, 2) = aPosition.y;
	myTransform(4, 3) = aPosition.z;
}

void Camera::Move(CommonUtilities::Vector3<float> aMovement)
{
	CommonUtilities::Vector4<float> aPosition = { aMovement.x,aMovement.y,aMovement.z,1 };
	aPosition = aPosition * myTransform;

	myTransform(4, 1) = aPosition.x;
	myTransform(4, 2) = aPosition.y;
	myTransform(4, 3) = aPosition.z;
}

void Camera::Rotate(CommonUtilities::Vector3<float> aRotation)
{
	CommonUtilities::Vector3<float> position = { myTransform(4,1),myTransform(4,2),myTransform(4,3) };

	CommonUtilities::Matrix4x4<float> mat = CommonUtilities::Matrix4x4<float>::CreateRotationAroundX(aRotation.x) * CommonUtilities::Matrix4x4<float>::CreateRotationAroundY(aRotation.y) * CommonUtilities::Matrix4x4<float>::CreateRotationAroundZ(aRotation.z);

	mat *= myTransform;
	myTransform = mat;

	myTransform(4, 1) = position.x;
	myTransform(4, 2) = position.y;
	myTransform(4, 3) = position.z;
}

void Camera::RotateWorldSpace(CommonUtilities::Vector3<float> aRotation)
{
	CommonUtilities::Vector3<float> position = { myTransform(4,1),myTransform(4,2),myTransform(4,3) };

	CommonUtilities::Matrix4x4<float> mat = CommonUtilities::Matrix4x4<float>::CreateRotationAroundX(aRotation.x) * CommonUtilities::Matrix4x4<float>::CreateRotationAroundY(aRotation.y) * CommonUtilities::Matrix4x4<float>::CreateRotationAroundZ(aRotation.z);

	myTransform *= mat;
	mat = myTransform;

	myTransform(4, 1) = position.x;
	myTransform(4, 2) = position.y;
	myTransform(4, 3) = position.z;
}

void Camera::LookAt(CommonUtilities::Vector3<float> aTarget)
{
	V3F pos(myTransform(4, 1), myTransform(4, 2), myTransform(4, 3));
	V3F forward = { aTarget - pos };
	V3F worldUp(0, 1, 0);
	V3F right = forward.Cross(-worldUp);
	V3F up = -right.Cross(forward);

	if (right.LengthSqr() < 0.5f)
	{
		right = V3F(0, 0, 1);
		up = V3F(1, 0, 0);
	}

	up.Normalize();
	right.Normalize();
	forward.Normalize();

	CommonUtilities::Matrix4x4<float> mat;

	mat(1, 1) = right.x;
	mat(1, 2) = right.y;
	mat(1, 3) = right.z;
	mat(2, 1) = up.x;
	mat(2, 2) = up.y;
	mat(2, 3) = up.z;
	mat(3, 1) = forward.x;
	mat(3, 2) = forward.y;
	mat(3, 3) = forward.z;
	mat(4, 1) = pos.x;
	mat(4, 2) = pos.y;
	mat(4, 3) = pos.z;
	mat(4, 4) = 1;
	myTransform = mat;
}

CommonUtilities::Matrix4x4<float> Camera::GetTransform() const 
{
	return myTransform;
}

CommonUtilities::Matrix4x4<float> Camera::GetProjection() const
{
	return myProjection;
}

CommonUtilities::Vector3<float> Camera::GetForward() const
{
	CommonUtilities::Vector4<float> forward(0, 0, 1, 0);
	forward = forward * myTransform;
	return CommonUtilities::Vector3<float>(forward.x, forward.y, forward.z);
}
V3F Camera::GetFlatForward() const
{
	CommonUtilities::Vector4<float> forward(0, 0, 1, 0);
	forward = forward * myTransform;
	return CommonUtilities::Vector3<float>(forward.x, 0.f, forward.z).GetNormalized();
}

CommonUtilities::Vector3<float> Camera::GetUp() const
{
	CommonUtilities::Vector4<float> up(0, 1, 0, 0);
	up = up * myTransform;
	return CommonUtilities::Vector3<float>(up.x, up.y, up.z);
}
CommonUtilities::Vector3<float> Camera::GetRight() const 
{
	CommonUtilities::Vector4<float> right(1, 0, 0, 0);
	right = right * myTransform;
	return CommonUtilities::Vector3<float>(right.x, right.y, right.z);
}

CommonUtilities::Vector3<float> Camera::GetPosition() const
{
	return CommonUtilities::Vector3<float>(myTransform(4, 1), myTransform(4, 2), myTransform(4, 3));
}

CommonUtilities::PlaneVolume<float> Camera::GenerateFrustum() const
{
	CommonUtilities::PlaneVolume<float> frustum;
	const float pi = 3.141596f;
	CommonUtilities::Plane<float> plane;
	CommonUtilities::Vector4<float> direction;
	CommonUtilities::Vector4<float> point;
	if (myIsOrthogonal)
	{
		V3F forward = GetForward();
		V3F right = GetRight();
		V3F up = GetUp();
		V3F point = GetPosition() + (forward * myOrthoBounds.z / 2.f);
		plane.InitWithPointAndNormal(point - right * myOrthoBounds.x/2.0f,-right);
		frustum.AddPlane(plane);

		plane.InitWithPointAndNormal(point + right * myOrthoBounds.x / 2.0f, right);
		frustum.AddPlane(plane);


		plane.InitWithPointAndNormal(point - up * myOrthoBounds.x / 2.0f, -up);
		frustum.AddPlane(plane);

		plane.InitWithPointAndNormal(point + up * myOrthoBounds.x / 2.0f, up);
		frustum.AddPlane(plane);


		plane.InitWithPointAndNormal(point - forward * myOrthoBounds.x / 2.0f, -forward);
		frustum.AddPlane(plane);

		plane.InitWithPointAndNormal(point + forward * myOrthoBounds.x / 2.0f, forward);
		frustum.AddPlane(plane);
		return frustum;
	}

	//https://docs.google.com/spreadsheets/d/13mytbFfr5WlF_QbJZcT4Zgctssh2ZbVmEWf1PjjlTIA/edit#gid=0
	float m = 1.00423f;
	float b = 0.031647f;
	float x = myResolution.x / myResolution.y;

	float yfov = (m / x + b) * myFOV;

	point = CommonUtilities::Vector4<float>(GetPosition().x, GetPosition().y, GetPosition().z, 2);

	direction = CommonUtilities::Vector4<float>(0.f, 0.f, 1.f, 0.f) * CommonUtilities::Matrix4x4<float>::CreateRotationAroundY((pi + TORAD(myFOV)) * 0.5f) * myTransform;
	plane.InitWithPointAndNormal({ point.x, point.y, point.z }, { direction.x, direction.y, direction.z });
	frustum.AddPlane(plane);

	direction = CommonUtilities::Vector4<float>(0.f, 0.f, 1.f, 0.f) * CommonUtilities::Matrix4x4<float>::CreateRotationAroundY((pi + TORAD(myFOV)) * -0.5f) * myTransform;
	plane.InitWithPointAndNormal({ point.x, point.y, point.z }, { direction.x, direction.y, direction.z });
	frustum.AddPlane(plane);

	direction = CommonUtilities::Vector4<float>(0.f, 0.f, 1.f, 0.f) * CommonUtilities::Matrix4x4<float>::CreateRotationAroundX((pi + TORAD(yfov)) * -0.5f) * myTransform;
	plane.InitWithPointAndNormal({ point.x, point.y, point.z }, { direction.x, direction.y, direction.z });
	frustum.AddPlane(plane);

	direction = CommonUtilities::Vector4<float>(0.f, 0.f, 1.f, 0.f) * CommonUtilities::Matrix4x4<float>::CreateRotationAroundX((pi + TORAD(yfov)) * 0.5f) * myTransform;
	plane.InitWithPointAndNormal({ point.x, point.y, point.z }, { direction.x, direction.y, direction.z });
	frustum.AddPlane(plane);

	point = CommonUtilities::Vector4<float>(0, 0, myFarPlane, 0) * myTransform + CommonUtilities::Vector4<float>(GetPosition().x, GetPosition().y, GetPosition().z, 0);
	direction = CommonUtilities::Vector4<float>(0.f, 0.f, 1.f, 0.f) * myTransform;
	plane.InitWithPointAndNormal({ point.x, point.y, point.z }, { direction.x, direction.y, direction.z });
	frustum.AddPlane(plane);

	point = CommonUtilities::Vector4<float>(0, 0, myNearPlane, 0) * myTransform + CommonUtilities::Vector4<float>(GetPosition().x, GetPosition().y, GetPosition().z, 0);
	direction = CommonUtilities::Vector4<float>(0.f, 0.f, -1.f, 0.f) * myTransform;
	plane.InitWithPointAndNormal({ point.x, point.y, point.z }, { direction.x, direction.y, direction.z });
	frustum.AddPlane(plane);

	return frustum;
}
