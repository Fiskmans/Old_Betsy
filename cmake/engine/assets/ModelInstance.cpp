#include "engine/assets/ModelInstance.h"

#include "engine/assets/Model.h"

#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/RenderManager.h"
#include "engine/graphics/ShaderBuffers.h"

#include "tools/Logger.h"

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

	ModelInstance::ModelInstance(const AssetHandle<ModelAsset>& aModel)
	{
		if (!aModel.Is<ModelAsset>())
		{
			LOG_SYS_CRASH("Asset was not a model");
		}
		myId = ++ourIdCounter;
		myModel = aModel;
		myScale = { 1.0f, 1.0f, 1.0f };
		myGraphicBoundsModifier = 1.f;
		myAnimator = nullptr;
		myShouldRender = true;
		mySpawnTime = Time::Now();
		SetCastsShadows(true);
	}

	const ModelAsset& ModelInstance::GetModelAsset() const 
	{
		return myModel.Access();
	}

	tools::M44f ModelInstance::GetModelToWorldTransform()
	{
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
		return !!(myModel.Access().myModel->GetModelData()[0]->myshaderTypeFlags & ShaderFlags::HasBones);
	}


	bool ModelInstance::ShouldRender() const
	{
		return myShouldRender;
	}

	tools::V4f ModelInstance::GetPosition()
	{
		return myTransform.Row(3);
	}

	tools::Sphere<float> ModelInstance::GetBoundingSphere(float aRangeModifier)
	{
		return tools::Sphere<float>(myTransform.Row(3), myModel.Access().myModel->GetSize() * myGraphicBoundsModifier * aRangeModifier);
	}

	const time::Seconds ModelInstance::GetSpawnTime()
	{
		return mySpawnTime;
	}

	void ModelInstance::SetIsHighlighted(bool aState)
	{
		myIsHighlighted = aState;
	}

	bool ModelInstance::GetIsHighlighted()
	{
		return myIsHighlighted;
	}

}