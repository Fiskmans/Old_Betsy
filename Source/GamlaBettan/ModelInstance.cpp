#include "pch.h"
#include "ModelInstance.h"
#include "Model.h"
#include "ShaderFlags.h"
#include "Animator.h"
#include "TimeHelper.h"

#include <GBPhysX.h>

#if USEIMGUI
#include "Camera.h"
#endif // !_RETAIL

int ModelInstance::myGlobalEventStatus = 0;

ModelInstance::ModelInstance(const AssetHandle& aModel) : myGaphicBoundsModifier(1.f)
{
	static unsigned int ids;
	myFriendlyName = "[" + std::to_string(ids++) + "] ";

	if (aModel.GetType() != Asset::AssetType::Model && aModel.GetType() != Asset::AssetType::SkyBox)
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
	static std::array<M44f, NUMBEROFANIMATIONBONES> transforms;
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
		const M44f& finalTrans = transforms[i];
		const M44f& offset = boneData[i].BoneOffset;
		M44f total = finalTrans * M44f::GetRealInverse(offset);
		positions[i] = positions[i] * M44f::Transpose(total);
		positions[i] = positions[i] * toWorld;
	}

	return positions;
}

CommonUtilities::Matrix4x4<float> ModelInstance::GetModelToWorldTransform()
{
	//CommonUtilities::Matrix4x4<float> mat = myScaleAndRotate;
	CommonUtilities::Matrix4x4<float> toWorld = myTransform;

	CommonUtilities::Matrix4x4<float> scale;
	scale(1, 1) = myScale.x;
	scale(2, 2) = myScale.y;
	scale(3, 3) = myScale.z;

	toWorld *= scale;

	return toWorld;
}

M44f ModelInstance::GetModelToWorldTransformWithPotentialBoneAttachement(BoneTextureCPUBuffer& aBoneData, std::unordered_map<ModelInstance*, short>& aBoneMapping)
{
	if (myIsAttachedToBone)
	{
		M44f base = myAttachedToModel->GetModelToWorldTransformWithPotentialBoneAttachement(aBoneData,aBoneMapping);
		M44f bonedata = aBoneData[aBoneMapping[myAttachedToModel]][myBoneIndex];

		return M44f::Transpose(bonedata) * base;
	}
	return GetModelToWorldTransform();
}

void ModelInstance::SetPosition(const CommonUtilities::Vector4<float>& aPosition)
{
	myTransform.AssignRow(3, aPosition);
}

void ModelInstance::Rotate(CommonUtilities::Vector3<float> aRotation)
{
	myTransform *= M44f::CreateRotation(aRotation);
}

void ModelInstance::Rotate(const CommonUtilities::Matrix4x4<float>& aRotationMatrix)
{
	myTransform *= aRotationMatrix;
}

void ModelInstance::SetRotation(CommonUtilities::Vector3<float> aRotation)
{
	V4F pos = myTransform.Row(3);
	myTransform = M44f::CreateRotation(aRotation);

	myTransform.AssignRow(3, pos);
}

void ModelInstance::SetRotation(const CommonUtilities::Matrix4x4<float>& aTargetRotation)
{
	V4F pos = myTransform.Row(3);
	myTransform =  aTargetRotation;

	myTransform.AssignRow(3, pos);
}

void ModelInstance::SetScale(CommonUtilities::Vector3<float> aScale)
{
	myScale = aScale;
	myGaphicBoundsModifier = MAX(MAX(aScale.x, aScale.y), aScale.z);
}

void ModelInstance::SetTransform(const M44f& aTransform)
{
	myTransform = aTransform;
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

void ModelInstance::SetGBPhysXCharacter(GBPhysXCharacter* aGBPhysXCharacter)
{
	myGBPhysXCharacter = aGBPhysXCharacter;
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
	return myTransform.Row(3);
}

CommonUtilities::Vector3<float> ModelInstance::GetPosition() const
{
	return CommonUtilities::Vector3<float>(myTransform.Row(3));
}

const std::string ModelInstance::GetFriendlyName()
{
	return myFriendlyName + myModel.GetAsModel()->GetFriendlyName();
}

void ModelInstance::SetupanimationMatrixes(std::array<CommonUtilities::Matrix4x4<float>, NUMBEROFANIMATIONBONES>& aMatrixes)
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
	return CommonUtilities::Sphere<float>(myTransform.Row(3), myModel.GetAsModel()->GetGraphicSize() * myGaphicBoundsModifier * aRangeModifier);
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