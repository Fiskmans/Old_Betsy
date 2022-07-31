#include "engine/assets/ModelInstance.h"

#include "engine/assets/Model.h"

#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/RenderManager.h"
#include "engine/graphics/ShaderBuffers.h"

#include "tools/TimeHelper.h"

#include "logger/Logger.h"

//#include "Model.h"
//#include "ShaderFlags.h"
//#include "Animator.h"
//#include "TimeHelper.h"


namespace engine
{
	ID3D11Buffer* ModelInstance::ourAnimationBuffer = nullptr;
	unsigned int ModelInstance::ourIdCounter = 0;

	bool ModelInstance::InitShared()
	{
		if (!graphics::RenderManager::CreateGenericShaderBuffer<graphics::AnimationBuffer>(ourAnimationBuffer))
		{
			LOG_SYS_CRASH("Failed to create animation buffer");
			return false;
		}

		return true;
	}

	void ModelInstance::ReleaseShared()
	{
		SAFE_RELEASE(ourAnimationBuffer);
	}

	ModelInstance::ModelInstance(const AssetHandle& aModel)
	{
		if (!aModel.Is<ModelAsset>() && !aModel.Is<SkyboxAsset>())
		{
			LOG_SYS_CRASH("Asset was not a model");
		}
		myId = ++ourIdCounter;
		myModel = aModel;
		myScale = { 1.0f,1.0f,1.0f };
		myAnimator = nullptr;
		myShouldRender = true;
		SetCastsShadows(true);
		ResetSpawnTime();
	}

	const ModelAsset& ModelInstance::GetModelAsset() const 
	{
		return myModel.Get<ModelAsset>();
	}

	void ModelInstance::ResetSpawnTime()
	{
		mySpawnTime = tools::GetTotalTime();
	}

	tools::M44f ModelInstance::GetModelToWorldTransform()
	{
		//tools::M44f mat = myScaleAndRotate;
		tools::M44f toWorld = myTransform;

		tools::M44f scale;
		scale.Row(1)[1] = myScale[0];
		scale.Row(2)[2] = myScale[1];
		scale.Row(3)[3] = myScale[2];

		toWorld *= scale;

		return toWorld;
	}

	void ModelInstance::SetPosition(const tools::V4f& aPosition)
	{
		myTransform.Row(3) = aPosition;
	}

	void ModelInstance::Rotate(tools::V3f aRotation)
	{
		myTransform *= tools::M44f::CreateRotation(aRotation);
	}

	void ModelInstance::Rotate(const tools::M44f& aRotationMatrix)
	{
		myTransform *= aRotationMatrix;
	}

	void ModelInstance::SetRotation(tools::V3f aRotation)
	{
		tools::V4f pos = myTransform.Row(3);
		myTransform = tools::M44f::CreateRotation(aRotation);

		myTransform.Row(3) = pos;
	}

	void ModelInstance::SetRotation(const tools::M44f& aTargetRotation)
	{
		tools::V4f pos = myTransform.Row(3);
		myTransform = aTargetRotation;

		myTransform.Row(3) = pos;
	}

	void ModelInstance::SetScale(tools::V3f aScale)
	{
		myScale = aScale;
	}

	void ModelInstance::SetTransform(const tools::M44f& aTransform)
	{
		myTransform = aTransform;
	}

	void ModelInstance::SetShouldRender(const bool aFlag)
	{
		myShouldRender = aFlag;
	}

	//void ModelInstance::AttachAnimator(Animator* aAnimator)
	//{
	//	myAnimator = aAnimator;
	//}

	bool ModelInstance::HasAnimations() const
	{
		return !!(myModel.Get<ModelAsset>().myModel->GetModelData()[0]->myshaderTypeFlags & ShaderFlags::HasBones);
	}


	bool ModelInstance::ShouldRender() const
	{
		return myShouldRender;
	}

	tools::V4f ModelInstance::GetPosition()
	{
		return myTransform.Row(3);
	}

	void ModelInstance::SetupanimationMatrixes()
	{
		thread_local graphics::AnimationBuffer bones;
		//if (myAnimator)
		//{
		//	myAnimator->BoneTransform(aMatrixes);
		//}
		//else
		//{
		//	static bool onetimeWarning = true;
		//	if (onetimeWarning)
		//	{
		//		SYSWARNING("Model instance with bones tried to animate without an attached controller", myModel.Get<ModelAsset>().myFilePath);
		//		onetimeWarning = false;
		//	}
		//}
		graphics::RenderManager::OverWriteBuffer(ourAnimationBuffer, bones.myTransforms, sizeof(graphics::AnimationBuffer));
	}

	/*
	bool ModelInstance::ImGuiNode(std::map<std::string, std::vector<std::string>>& aFiles, Camera* aCamera)
	{
		tools::V3f pos = GetPosition();
		if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
		{
			SetPosition({ pos.x,pos.y,pos.z,1 });
		}
		float rotation[3] = { 0,0,0 };
		if (ImGui::DragFloat3("Rotate", rotation, 0.01f, 0.0f, 0.0f, "%.1f"))
		{
			Rotate(tools::V3f(rotation[0], rotation[1], rotation[2]));
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
			ImGui::TextColored(ImColor(1, 0, 0), "%s", "Model not loaded yet");
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
			aCamera->SetPosition(pos + tools::V3f(0, 0, -3));
			aCamera->SetRotation(CommonUtilities::Matrix3x3<float>());
		}
		ImGui::SameLine();

		if (ImGui::Button("Remove"))
		{
			return true;
		}
		return false;
	}
	*/

	tools::Sphere<float> ModelInstance::GetBoundingSphere(float aRangeModifier)
	{
		return tools::Sphere<float>(myTransform.Row(3), myModel.Get<ModelAsset>().myModel->GetSize() * myGraphicBoundsModifier * aRangeModifier);
	}

	const float ModelInstance::GetSpawnTime()
	{
		return mySpawnTime;
	}

	void ModelInstance::SetIsHighlighted(bool aState)
	{
		myIsHighlighted = aState;
	}

	bool ModelInstance::GetIsHighlighted()
	{
		for (Model::ModelData* data : myModel.Get<ModelAsset>().myModel->GetModelData())
		{
			if (!data->myTextures[0].IsValid()) { return false; }
			if (!data->myTextures[1].IsValid()) { return false; }
			if (!data->myTextures[2].IsValid()) { return false; }
			if (!data->myTextures[0].Is<TextureAsset>()) { return false; }
			if (!data->myTextures[1].Is<TextureAsset>()) { return false; }
			if (!data->myTextures[2].Is<TextureAsset>()) { return false; }
		}
		return myIsHighlighted;
	}

}