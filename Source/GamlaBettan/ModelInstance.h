#pragma once
#include "CommonUtilities/Sphere.hpp"

class Camera;
class Animator;
class GBPhysXCharacter;

class ModelInstance
{
public:
	ModelInstance(const AssetHandle& aModel);
	AssetHandle& GetModelAsset();
	M44f GetModelToWorldTransform();
	M44f GetModelToWorldTransformWithPotentialBoneAttachement(BoneTextureCPUBuffer& aBoneData, std::unordered_map<ModelInstance*, short>& aBoneMapping);
	void SetPosition(const V4F& aPosition);
	void Rotate(V3F aRotation);
	void Rotate(const M44f& aRotationMatrix);
	void SetRotation(V3F aRotation);
	void SetRotation(const M44f& aTargetRotation);
	void SetTransform(const M44f& aTransform);
	void SetScale(V3F aScale);
	void SetShouldBeDrawnThroughWalls(const bool aFlag);
	void SetUsePlayerThroughWallShader(const bool aFlag);
	void SetShouldRender(const bool aFlag);
	void AttachAnimator(Animator* aAnimator);
	void ResetSpawnTime();
	void AttachToBone(ModelInstance* aParentModel, size_t aBone);
	void SetCastsShadows(bool aValue);

	std::array<V4F, NUMBEROFANIMATIONBONES> GetBonePositions();

	void SetGBPhysXCharacter(GBPhysXCharacter* aGBPhysXCharacter);
	
	bool HasAnimations();

	void SetTint(V4F aTint);
	V4F GetTint();

	bool ShouldBeDrawnThroughWalls() const;
	bool IsUsingPlayerThroughWallShader() const;
	bool ShouldRender() const;
	bool GetCastsShadows() const;

	const V4F& GetV4FPosition() const;
	V3F GetPosition() const;

	const std::string GetFriendlyName();

	void SetupanimationMatrixes(std::array<M44f,NUMBEROFANIMATIONBONES>& aMatrixes);

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

	void Interact();
	float GetLastInteraction();

	static void SetGlobalEventStart();
	static void SetGlobalEventEnd();
	static int GetEventStatus();
private:

	static int myGlobalEventStatus;

	bool myIsHighlighted = false;

	float myCustomData[MODELSAMOUNTOFCUSTOMDATA*2];

	std::string myFriendlyName;

	M44f myTransform;

	V4F myTint;

	V3F myScale;

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