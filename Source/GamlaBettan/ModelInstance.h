#pragma once
#include <Matrix3x3.hpp>
#include <Matrix4x4.hpp>
#include <Vector3.hpp>
#include <Vector4.hpp>
#include <Sphere.hpp>
#include "Asset.h"

#if USEIMGUI
#include <map>
#include <vector>
#include <string>
#endif

class Camera;
class Animator;
class GBPhysXCharacter;

class ModelInstance
{
public:
	ModelInstance(AssetHandle& aModel);
	AssetHandle& GetModelAsset();
	CommonUtilities::Matrix4x4<float> GetModelToWorldTransform();
	M44F GetModelToWorldTransformWithPotentialBoneAttachement(BoneTextureCPUBuffer& aBoneData, std::unordered_map<ModelInstance*, short>& aBoneMapping);
	void SetPosition(const CommonUtilities::Vector4<float>& aPosition);
	void Rotate(CommonUtilities::Vector3<float> aRotation);
	void Rotate(const CommonUtilities::Matrix4x4<float>& aRotationMatrix);
	void SetRotation(CommonUtilities::Vector3<float> aRotation);
	void SetRotation(const CommonUtilities::Matrix4x4<float>& aTargetRotation);
	void SetScale(CommonUtilities::Vector3<float> aScale);
	void SetShouldBeDrawnThroughWalls(const bool aFlag);
	void SetUsePlayerThroughWallShader(const bool aFlag);
	void SetShouldRender(const bool aFlag);
	void AttachAnimator(Animator* aAnimator);
	void SetAnimation(int aAnimation);
	void StepAnimation(float aDeltaTime);
	void ResetSpawnTime();
	void AttachToBone(ModelInstance* aParentModel, size_t aBone);
	void SetCastsShadows(bool aValue);

	std::array<V4F, NUMBEROFANIMATIONBONES> GetBonePositions();
	M44F GetTransformOfBone(int aIndex);

	//TODO RAGDOLL
	
	//void DetachAnimator();
	void SetGBPhysXCharacter(GBPhysXCharacter* aGBPhysXCharacter);
	
	bool HasAnimations();

	void SetTint(V4F aTint);
	V4F GetTint();

	bool ShouldBeDrawnThroughWalls() const;
	bool IsUsingPlayerThroughWallShader() const;
	bool ShouldRender() const;
	bool GetCastsShadows() const;

	const CommonUtilities::Vector4<float>& GetV4FPosition() const;
	CommonUtilities::Vector3<float> GetPosition() const;

	const std::string GetFriendlyName();

	void SetupanimationMatrixes(std::array<CommonUtilities::Matrix4x4<float>,NUMBEROFANIMATIONBONES>& aMatrixes);

#if USEIMGUI
	virtual bool ImGuiNode(std::map<std::string, std::vector<std::string>>& aFiles, Camera* aCamera);
#endif // !_RETAIL

	CommonUtilities::Sphere<float> GetGraphicBoundingSphere(float aRangeModifier = 1.f);
	const float GetSpawnTime();
	void SetExpectedLifeTime(float aLifeTime);
	const float GetExpectedLifeTime();

	void SetCustomPixelData(float aData[MODELSAMOUNTOFCUSTOMDATA]);
	void SetCustomVertexData(float aData[MODELSAMOUNTOFCUSTOMDATA]);

	void SetIsHighlighted(bool aState);
	bool GetIsHighlighted();

	void SetUsingSecondaryFov(bool aState);
	bool GetIsUsingSecondaryFov();

	void Interact();
	float GetLastInteraction();

	static void SetGlobalEventStart();
	static void SetGlobalEventEnd();
	static int GetEventStatus();
private:

	static int myGlobalEventStatus;

	bool myIsHighlighted = false;
	bool myIsUsingSecondaryFov = false;

	float myCustomData[MODELSAMOUNTOFCUSTOMDATA*2];

	std::string myFriendlyName;
	//CommonUtilities::Matrix4x4<float> myScaleAndRotate;
	CommonUtilities::Matrix4x4<float> myRotation;

	CommonUtilities::Vector4<float> myPosition;
	V4F myTint;

	CommonUtilities::Vector3<float> myScale;

	class Animator* myAnimator;
	AssetHandle myModel;

	float myGaphicBoundsModifier;

	bool myShouldBeDrawnThroughWalls;
	bool myUsePlayerThroughWallShader;
	bool myShouldRender;
	bool myCastsShadows;
	float mySpawnTime;
	float myExpectedLifeTime;
	float myLastInteraction = -1.f;


	bool myIsAttachedToBone;
	ModelInstance* myAttachedToModel;
	int myBoneIndex;

	GBPhysXCharacter* myGBPhysXCharacter = nullptr;

};

inline void ModelInstance::SetCastsShadows(bool aValue)
{
	myCastsShadows = aValue;
}

inline bool ModelInstance::GetCastsShadows() const
{
	return myCastsShadows;
}