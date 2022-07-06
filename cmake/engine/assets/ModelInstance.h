#ifndef ENGINE_ASSETS_MODEL_INSTANCE_H
#define ENGINE_ASSETS_MODEL_INSTANCE_H
//#include "CommonUtilities/Sphere.hpp"

#include "tools/MathVector.h"
#include "tools/Matrix4x4.h"
#include "tools/Sphere.h"

#include "engine/graphics/BoneBuffer.h"

#include "engine/assets/Asset.h"

#include <unordered_map>

namespace engine
{
	class ModelInstance
	{
	public:
		ModelInstance(const engine::AssetHandle& aModel);
		engine::AssetHandle& GetModelAsset();
		tools::M44f GetModelToWorldTransform();
		void SetPosition(const tools::V4f& aPosition);
		void Rotate(tools::V3f aRotation);
		void Rotate(const tools::M44f& aRotationMatrix);
		void SetRotation(tools::V3f aRotation);
		void SetRotation(const tools::M44f& aTargetRotation);
		void SetTransform(const tools::M44f& aTransform);
		void SetScale(tools::V3f aScale);
		void SetShouldRender(const bool aFlag);
		//void AttachAnimator(Animator* aAnimator);
		void ResetSpawnTime();
		void AttachToBone(engine::ModelInstance* aParentModel, size_t aBone);
		void SetCastsShadows(bool aValue);

		std::array<tools::V4f, NUMBEROFANIMATIONBONES> GetBonePositions();


		bool ShouldRender() const;
		bool HasAnimations() const;
		bool GetCastsShadows() const;

		tools::V4f GetPosition();

		void SetupanimationMatrixes(std::array<tools::M44f, NUMBEROFANIMATIONBONES>& aMatrixes);

		tools::Sphere<float> GetBoundingSphere(float aRangeModifier = 1.f);
		const float GetSpawnTime();

		void SetIsHighlighted(bool aState);
		bool GetIsHighlighted();

	private:

		bool myIsHighlighted = false;

		tools::M44f myTransform;

		tools::V3f myScale;

		class Animator* myAnimator;
		engine::AssetHandle myModel;

		float myGraphicBoundsModifier;

		bool myShouldRender;
		bool myCastsShadows;
		float mySpawnTime;
	};

	inline void ModelInstance::SetCastsShadows(bool aValue)
	{
		myCastsShadows = aValue;
	}

	inline bool ModelInstance::GetCastsShadows() const
	{
		return myCastsShadows;
	}
}

#endif