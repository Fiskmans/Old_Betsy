#include "pch.h"
#include "ModelInstance.h"
#include "Model.h"
#include "ShaderFlags.h"
#include "Animator.h"
#include "TimeHelper.h"
//#pragma push_macro(MIN)
//#pragma push_macro(MAX)

#include <GBPhysX.h>
//#pragma pop_macro(MIN)
//#pragma pop_macro(MAX)

#if USEIMGUI
#include "Camera.h"
#include <imgui.h>
#endif // !_RETAIL

int ModelInstance::myGlobalEventStatus = 0;

ModelInstance::ModelInstance(AssetHandle& aModel) : myGaphicBoundsModifier(1.f)
{
	static unsigned int ids;
	myFriendlyName = "[" + std::to_string(ids++) + "] ";

	if (aModel.GetType() != Asset::AssetType::Model)
	{
		SYSCRASH("Asset was not a model");
	}
	myModel = aModel;
	myScale = { 1.0f,1.0f,1.0f };
	myAnimator = nullptr;
	myTint = V4F(0.0f, 0.0f, 0.0f, 1.0f);
	myShouldBeDrawnThroughWalls = false;
	myUsePlayerThroughWallShader = false;
	myShouldRender = true;
	SetCastsShadows(true);
	myExpectedLifeTime = 0.0f;
	ResetSpawnTime();
}

AssetHandle& ModelInstance::GetModelAsset()
{
	return myModel;
}

void ModelInstance::ResetSpawnTime()
{
	mySpawnTime = Tools::GetTotalTime();
}

std::array<V4F, NUMBEROFANIMATIONBONES> ModelInstance::GetBonePositions()
{
	static std::array<M44F, NUMBEROFANIMATIONBONES> transforms;
	SetupanimationMatrixes(transforms);
	static std::array<V4F, NUMBEROFANIMATIONBONES> positions;
	for (auto& i : positions)
	{
		i = V4F(0, 0, 0, 1);
	}

	auto& boneData = myModel.GetAsModel()->myBoneData;
	auto toWorld = GetModelToWorldTransform();

	for (size_t i = 0; i < boneData.size(); i++)
	{
		const M44F& finalTrans = transforms[i];
		const M44F& offset = boneData[i].BoneOffset;
		M44F total = finalTrans * M44F::GetRealInverse(offset);
		positions[i] = positions[i] * M44F::Transpose(total);
		positions[i] = positions[i] * toWorld;
	}

	return positions;
}

CommonUtilities::Matrix4x4<float> ModelInstance::GetModelToWorldTransform()
{
	//CommonUtilities::Matrix4x4<float> mat = myScaleAndRotate;
	CommonUtilities::Matrix4x4<float> toWorld = myRotation;

	CommonUtilities::Matrix4x4<float> scale;
	scale(1, 1) = myScale.x;
	scale(2, 2) = myScale.y;
	scale(3, 3) = myScale.z;

	toWorld *= scale;

	toWorld(4, 1) = myPosition.x;
	toWorld(4, 2) = myPosition.y;
	toWorld(4, 3) = myPosition.z;

	return toWorld;
}
struct Space
{
	M44F* matrix;
	const char* name;
	bool included = true;
	bool inversed = false;
	bool transposed = false;
};
M44F ModelInstance::GetModelToWorldTransformWithPotentialBoneAttachement(BoneTextureCPUBuffer& aBoneData, std::unordered_map<ModelInstance*, short>& aBoneMapping)
{
	if (myIsAttachedToBone)
	{
		M44F base = myAttachedToModel->GetModelToWorldTransformWithPotentialBoneAttachement(aBoneData,aBoneMapping);
		M44F bonedata = aBoneData[aBoneMapping[myAttachedToModel]][myBoneIndex];

		return M44F::Transpose(bonedata) * base;
		
#if USEIMGUI
		DebugDrawer::GetInstance().DrawArrow(V3F(0, 0, 0), V3F(V4F(0, 0, 0, 1) * M44F::Transpose(M44F::Transpose(bonedata) * base)));
		// used below to get ^^

		M44F offset = (*myAttachedToModel->myAnimator->GetBoneInfo())[myBoneIndex].BoneOffset;
		static std::array<Space,3> spaces;
		spaces[0].matrix = &base;
		spaces[1].matrix = &bonedata;
		spaces[2].matrix = &offset;
		spaces[0].name = "base";
		spaces[1].name = "bonedata";
		spaces[2].name = "offset";
		static std::vector<int> order = { 0,1,2 };

		M44F result = M44F::Identity();
		for (size_t i = 0; i < order.size(); i++)
		{
			ImGui::PushID(i);
			if (i!=0)
			{
				ImGui::Separator();
			}
			Space& space = spaces[order[i]];
			ImGui::Text(space.name);
			if (i != 0)
			{
				if (ImGui::Button("^"))
				{
					std::swap(order[i], order[i - 1]);
				}
			}
			ImGui::Checkbox("included", &space.included);
			ImGui::Checkbox("Inversed", &space.inversed);
			ImGui::Checkbox("Transposed", &space.transposed);
			if (space.included)
			{
				M44F mat = *space.matrix;

				if (space.transposed)
				{
					mat = M44F::Transpose(mat);
				}
				if (space.inversed)
				{
					mat = M44F::GetRealInverse(mat);
				}
				result = result *mat;
			}
			ImGui::PopID();
		}

		DebugDrawer::GetInstance().DrawGizmo(V3F(V4F(0, 0, 0, 1) * result), 30, result);
		DebugDrawer::GetInstance().DrawArrow(V3F(0, 0, 0), V3F(V4F(0, 0, 0, 1) * result));


		return result;
#endif
	}
	return GetModelToWorldTransform();
}

void ModelInstance::SetPosition(const CommonUtilities::Vector4<float>& aPosition)
{
	myPosition = aPosition;
}

void ModelInstance::Rotate(CommonUtilities::Vector3<float> aRotation)
{
	myRotation *= CommonUtilities::Matrix4x4<float>::CreateRotationAroundX(aRotation.x);
	myRotation *= CommonUtilities::Matrix4x4<float>::CreateRotationAroundY(aRotation.y);
	myRotation *= CommonUtilities::Matrix4x4<float>::CreateRotationAroundZ(aRotation.z);
}

void ModelInstance::Rotate(const CommonUtilities::Matrix4x4<float>& aRotationMatrix)
{
	myRotation *= aRotationMatrix;
}

void ModelInstance::SetRotation(CommonUtilities::Vector3<float> aRotation)
{
	CommonUtilities::Matrix4x4<float> mat;
	mat.CreateRotationAroundX(aRotation.x);
	mat.CreateRotationAroundY(aRotation.y);
	mat.CreateRotationAroundZ(aRotation.z);

	myRotation = mat;
}

void ModelInstance::SetRotation(const CommonUtilities::Matrix4x4<float>& aTargetRotation)
{
	myRotation = aTargetRotation;
}

void ModelInstance::SetScale(CommonUtilities::Vector3<float> aScale)
{
	myScale = aScale;
	myGaphicBoundsModifier = MAX(MAX(aScale.x, aScale.y), aScale.z);

	/*CommonUtilities::Matrix4x4<float> mat;
	mat(1, 1) = aScale.x;
	mat(2, 2) = aScale.y;
	mat(3, 3) = aScale.z;
	myScaleAndRotate *= mat;*/
}

void ModelInstance::SetShouldBeDrawnThroughWalls(const bool aFlag)
{
	myShouldBeDrawnThroughWalls = aFlag;
}

void ModelInstance::SetUsePlayerThroughWallShader(const bool aFlag)
{
	myUsePlayerThroughWallShader = aFlag;
}

void ModelInstance::SetShouldRender(const bool aFlag)
{
	myShouldRender = aFlag;
}

void ModelInstance::AttachAnimator(Animator* aAnimator)
{
	myAnimator = aAnimator;
}

void ModelInstance::SetAnimation(int aAnimation)
{
	if (myAnimator)
	{
		if (myAnimator->GetAnimationCount() > aAnimation)
		{
			myAnimator->SetState(aAnimation);
			myAnimator->SetBlend(0.f);
			myAnimator->SetTime(0.f);
		}
		else
		{
			SYSWARNING("Trying to play animation that does not exist: [" + std::to_string(aAnimation) + "] max is [" + std::to_string(myAnimator->GetAnimationCount()+1) + "]","")
		}
	}
	else
	{
		SYSWARNING("Trying to animate something without an animator",myModel.GetAsModel()->GetModelData()->myFilePath);
	}
}

void ModelInstance::StepAnimation(float aDeltaTime)
{
	if (myAnimator)
	{
		myAnimator->Step(aDeltaTime);
	}
	else
	{
		SYSERROR("Trying to step animation on model that doesnt have an animator", myModel.GetAsModel()->GetModelData()->myFilePath);
	}
}

//TODO RAGDOLL

//void ModelInstance::DetachAnimator()
//{
//	std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES> currentStateMatrixes;
//	myAnimator->BoneTransform(currentStateMatrixes);
//	myGBPhysXActor->UpdateRagDollMatrices(currentStateMatrixes);
//	myAnimator = nullptr;
//}

void ModelInstance::SetGBPhysXCharacter(GBPhysXCharacter* aGBPhysXCharacter)
{
	myGBPhysXCharacter = aGBPhysXCharacter;
}

M44F ModelInstance::GetTransformOfBone(int aIndex)
{
	if (!HasAnimations())
	{
		return M44F();
	}
	return myAnimator->TransformOfBone(aIndex);
}

bool ModelInstance::HasAnimations()
{
	return !!(myModel.GetAsModel()->GetModelData()->myshaderTypeFlags & ShaderFlags::HasBones);
}

void ModelInstance::SetTint(V4F aTint)
{
	myTint = aTint;
}

V4F ModelInstance::GetTint()
{
	return myTint;
}

bool ModelInstance::ShouldBeDrawnThroughWalls() const
{
	return myShouldBeDrawnThroughWalls;
}

bool ModelInstance::IsUsingPlayerThroughWallShader() const
{
	return myUsePlayerThroughWallShader;
}

bool ModelInstance::ShouldRender() const
{
	return myShouldRender;
}

const CommonUtilities::Vector4<float>& ModelInstance::GetV4FPosition() const
{
	return myPosition;
}

CommonUtilities::Vector3<float> ModelInstance::GetPosition() const
{
	return CommonUtilities::Vector3<float>(myPosition);
}

const std::string ModelInstance::GetFriendlyName()
{
	return myFriendlyName + myModel.GetAsModel()->GetFriendlyName();
}

void ModelInstance::SetupanimationMatrixes(std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>& aMatrixes)
{
	if (myGBPhysXCharacter == nullptr || myGBPhysXCharacter->GetIsRagDoll() == false)
	{
		if (myAnimator)
		{
			myAnimator->BoneTransform(aMatrixes);
		}
		else
		{
			static bool onetimeWarning = true;
			if (onetimeWarning)
			{
				SYSWARNING("Model instance with bones tried to animate without an attached controller", myModel.GetAsModel()->GetModelData()->myFilePath);
				onetimeWarning = false;
			}
		}
	}
	else
	{
		myGBPhysXCharacter->UpdateRagDollMatrices(aMatrixes);
	}
}

#if USEIMGUI
bool ModelInstance::ImGuiNode(std::map<std::string, std::vector<std::string>>& aFiles, Camera* aCamera)
{
	CommonUtilities::Vector3<float> pos = GetPosition();
	if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
	{
		SetPosition({ pos.x,pos.y,pos.z,1 });
	}
	float rotation[3] = { 0,0,0 };
	if (ImGui::DragFloat3("Rotate", rotation, 0.01f, 0.0f, 0.0f, "%.1f"))
	{
		Rotate(CommonUtilities::Vector3<float>(rotation[0], rotation[1], rotation[2]));
	}
	if (ImGui::DragFloat3("Scale", &myScale.x, 0.003f, 0.0f, 0.0f, "%.5f"))
	{
	}
	ImGui::ColorEdit4("Tint", &myTint.x);
	{

	}
	static std::bitset<NUMBEROFANIMATIONBONES> bonesToCross;
	if (myModel.IsLoaded())
	{
		if (myModel.GetAsModel()->myBoneNameLookup.size() > 0)
		{
			if (ImGui::Button("Bones"))
			{
				ImGui::OpenPopup("boneIndexer");
			}
			if (ImGui::BeginPopup("boneIndexer"))
			{
				for (auto& i : myModel.GetAsModel()->myBoneNameLookup)
				{
					bool state = bonesToCross[i.second];
					if (ImGui::Checkbox(i.first.c_str(), &state))
					{
						bonesToCross[i.second] = state;
					}
					if (ImGui::IsItemHovered())
					{
						DebugDrawer::GetInstance().DrawCross(GetBonePositions()[i.second], 20);
					}
				}
				ImGui::EndPopup();
			}
		}
	}
	else
	{
		ImGui::TextColored(ImColor(1, 0, 0), "%s","Model not loaded yet");
	}

	if (bonesToCross.any())
	{
		auto bonepos = GetBonePositions();
		for (size_t i = 0; i < NUMBEROFANIMATIONBONES; i++)
		{
			if (bonesToCross[i])
			{
				DebugDrawer::GetInstance().DrawCross(bonepos[i], 10);
			}
		}
	}



	if (ImGui::Button("Find"))
	{
		CommonUtilities::Vector3<float> pos = GetPosition();
		aCamera->SetPosition(pos + CommonUtilities::Vector3<float>(0, 0, -3));
		aCamera->SetRotation(CommonUtilities::Matrix3x3<float>());
	}
	ImGui::SameLine();

	if (ImGui::Button("Remove"))
	{
		return true;
	}
	return false;
}
#endif

CommonUtilities::Sphere<float> ModelInstance::GetGraphicBoundingSphere(float aRangeModifier)
{
	if (myIsAttachedToBone)
	{
		return myAttachedToModel->GetGraphicBoundingSphere(aRangeModifier);
	}
	return CommonUtilities::Sphere<float>(myPosition, myModel.GetAsModel()->GetGraphicSize() * myGaphicBoundsModifier * aRangeModifier);
}

const float ModelInstance::GetSpawnTime()
{
	return mySpawnTime;
}

void ModelInstance::SetExpectedLifeTime(float aLifeTime)
{
	myExpectedLifeTime = aLifeTime;
}

const float ModelInstance::GetExpectedLifeTime()
{
	return myExpectedLifeTime;
}

void ModelInstance::SetCustomPixelData(float aData[MODELSAMOUNTOFCUSTOMDATA])
{
	memcpy(myCustomData + MODELSAMOUNTOFCUSTOMDATA, aData, MODELSAMOUNTOFCUSTOMDATA);
}

void ModelInstance::SetCustomVertexData(float aData[MODELSAMOUNTOFCUSTOMDATA])
{
	memcpy(myCustomData, aData, MODELSAMOUNTOFCUSTOMDATA);
}

void ModelInstance::SetIsHighlighted(bool aState)
{
	myIsHighlighted = aState;
}

bool ModelInstance::GetIsHighlighted()
{
	return myIsHighlighted;
}

void ModelInstance::SetUsingSecondaryFov(bool aState)
{
	myIsUsingSecondaryFov = aState;
}

bool ModelInstance::GetIsUsingSecondaryFov()
{
	return myIsUsingSecondaryFov;
}

void ModelInstance::Interact()
{
	myLastInteraction = Tools::GetTotalTime();
}

float ModelInstance::GetLastInteraction()
{
	return myLastInteraction;
}

void ModelInstance::AttachToBone(ModelInstance* aParentModel, size_t aBone)
{
	myIsAttachedToBone = true;
	myAttachedToModel = aParentModel;
	myBoneIndex = aBone;
}
void ModelInstance::SetGlobalEventStart()
{
	myGlobalEventStatus = 1;
}
void ModelInstance::SetGlobalEventEnd()
{
	myGlobalEventStatus = 0;
}

int ModelInstance::GetEventStatus()
{
	return myGlobalEventStatus;
}