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
		static bool InitShared();
		static void ReleaseShared();

		ModelInstance(const AssetHandle<ModelAsset>& aModel);
		const ModelAsset& GetModelAsset() const;
		tools::M44f GetModelToWorldTransform();
		void SetPosition(const tools::V4f& aPosition);
		void Rotate(tools::V3f aRotation);
		void Rotate(const tools::M44f& aRotationMatrix);
		void SetRotation(tools::V3f aRotation);
		void SetRotation(const tools::M44f& aTargetRotation);
		void SetTransform(const tools::M44f& aTransform);
		void SetScale(tools::V3f aScale);
		void SetShouldRender(const bool aFlag);
		void SetCastsShadows(bool aValue);

		bool ShouldRender() const;
		bool HasAnimations() const;
		bool GetCastsShadows() const;

		tools::V4f GetPosition();
		unsigned int GetId() const { return myId; }

		tools::Sphere<float> GetBoundingSphere(float aRangeModifier = 1.f);
		const float GetSpawnTime();

		void SetIsHighlighted(bool aState);
		bool GetIsHighlighted();

	private:

		static ID3D11Buffer* ourAnimationBuffer;
		static unsigned int ourIdCounter;

		unsigned int myId;

		bool myIsHighlighted = false;

		tools::M44f myTransform = tools::M44f::Identity();

		tools::V3f myScale;

		class Animator* myAnimator;
		engine::AssetHandle<ModelAsset> myModel;

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