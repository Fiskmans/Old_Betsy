#include "engine/graphics/RenderScene.h"

#include "engine/assets/PointLight.h"

#include "common/Macros.h"

#include "tools/Intersection.h"
#include "tools/Logger.h"
#include "tools/Time.h"


//#include "ParticleInstance.h"
//#include "CommonUtilities\Ray.hpp"
//#include "CommonUtilities\Intersection.hpp"
//#include "CommonUtilities\Sphere.hpp"
//#include "SpriteInstance.h"
//#include "TextInstance.h"

namespace engine
{

	void RenderScene::Update(float aDeltaTime)
	{
		//{
		//	PERFORMANCETAG("Particles");
		//	for (auto& i : myParticles)
		//	{
		//		i->Update(aDeltaTime, myMainCamera->GetPosition());
		//	}
		//}
		//for (int i = static_cast<int>(myParticles.size()) - 1; i >= 0; i--)
		//{
		//	if (myParticles[i]->IsDead() && !myParticles[i]->IsEternal())
		//	{
		//		RemoveFromScene(myParticles[i]);
		//		delete myParticles[i];
		//	}
		//}
	}

	void RenderScene::ImGui()
	{
		std::vector<tools::V3f> pointsOfInterest;
		
		if (ImGui::TreeNode("Cameras"))
		{
			ImGui::PushID("main");

			ImGui::Text("Main camera");

			ImGui::Text("Transform:");
			ImGui::Indent();
				ImGui::Text("%.01f %.01f %.01f %.01f", myMainCamera->GetTransform().Row(0)[0], myMainCamera->GetTransform().Row(0)[1], myMainCamera->GetTransform().Row(0)[2], myMainCamera->GetTransform().Row(0)[3]);
				ImGui::Text("%.01f %.01f %.01f %.01f", myMainCamera->GetTransform().Row(1)[0], myMainCamera->GetTransform().Row(1)[1], myMainCamera->GetTransform().Row(1)[2], myMainCamera->GetTransform().Row(1)[3]);
				ImGui::Text("%.01f %.01f %.01f %.01f", myMainCamera->GetTransform().Row(2)[0], myMainCamera->GetTransform().Row(2)[1], myMainCamera->GetTransform().Row(2)[2], myMainCamera->GetTransform().Row(2)[3]);
				ImGui::Text("%.01f %.01f %.01f %.01f", myMainCamera->GetTransform().Row(3)[0], myMainCamera->GetTransform().Row(3)[1], myMainCamera->GetTransform().Row(3)[2], myMainCamera->GetTransform().Row(3)[3]);
			ImGui::Unindent();
			ImGui::Text("Projection:");
			ImGui::Indent();
				ImGui::Text("%.01f %.01f %.01f %.01f", myMainCamera->GetProjection().Row(0)[0], myMainCamera->GetProjection().Row(0)[1], myMainCamera->GetProjection().Row(0)[2], myMainCamera->GetProjection().Row(0)[3]);
				ImGui::Text("%.01f %.01f %.01f %.01f", myMainCamera->GetProjection().Row(1)[0], myMainCamera->GetProjection().Row(1)[1], myMainCamera->GetProjection().Row(1)[2], myMainCamera->GetProjection().Row(1)[3]);
				ImGui::Text("%.01f %.01f %.01f %.01f", myMainCamera->GetProjection().Row(2)[0], myMainCamera->GetProjection().Row(2)[1], myMainCamera->GetProjection().Row(2)[2], myMainCamera->GetProjection().Row(2)[3]);
				ImGui::Text("%.01f %.01f %.01f %.01f", myMainCamera->GetProjection().Row(3)[0], myMainCamera->GetProjection().Row(3)[1], myMainCamera->GetProjection().Row(3)[2], myMainCamera->GetProjection().Row(3)[3]);
			ImGui::Unindent();



			ImGui::PopID();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Models"))
		{
			for (ModelInstance* model : myModels)
			{
				ImGui::PushID(model);

				ImGui::Text(model->GetModelAsset().myFilePath.c_str());

				ImGui::PopID();
			}

			ImGui::TreePop();
		}
	}

	//void RenderScene::RefreshAll(float aAmount)
	//{
		//for (ParticleInstance*& instance : myParticles)
		//{
		//	instance->RefreshTimeout(aAmount);
		//}
	//}

	void RenderScene::AddToScene(ModelInstance* aModel)
	{
		PERFORMANCETAG("Added model to scene");
		myModels.push_back(aModel);
	}

	//void RenderScene::AddToScene(SpotLight* aSpotLight)
	//{
	//	mySpotlights.push_back(aSpotLight);
	//}
	//
	//void RenderScene::AddToScene(Decal* aDecal)
	//{
	//	myDecals.push_back(aDecal);
	//}
	//
	//void RenderScene::AddToScene(SpriteInstance* aSprite)
	//{
	//	if (aSprite)
	//	{
	//		if (!aSprite->HasBeenAddedToScene())
	//		{
	//			mySprites.push_back(aSprite);
	//			aSprite->AddToScene();
	//		}
	//		else
	//		{
	//			LOG_SYS_WARNING("Sprite instance added to scene more than once!");
	//		}
	//	}
	//}
	//
	//void RenderScene::AddToScene(TextInstance* aText)
	//{
	//	if (!aText->HadBeenAddedToScene())
	//	{
	//		myTexts.push_back(aText);
	//		aText->SetAddedToSceneStatus(true);
	//	}
	//}

	void RenderScene::RemoveFromScene(ModelInstance* aModel)
	{
		decltype(myModels)::iterator at = std::find(myModels.begin(), myModels.end(), aModel);
		if (at != myModels.end())
		{
			myModels.erase(at);
		}
		else
		{
			LOG_ERROR("Trying to remove model that is not currently in the scene");
		}
	}

	void RenderScene::AddToScene(PointLight* aLight)
	{
		myPointLights.push_back(aLight);
	}

	void RenderScene::RemoveFromScene(PointLight* aLight)
	{
		auto it = std::find(myPointLights.begin(), myPointLights.end(), aLight);

		if (it != myPointLights.end())
		{
			myPointLights.erase(it);
			return;
		}

		LOG_SYS_WARNING("Tried to remove invalid point light :c");
	}

	//void RenderScene::RemoveFromScene(ParticleInstance* aParticle)
	//{
	//	for (size_t i = 0; i < myParticles.size(); i++)
	//	{
	//		if (myParticles[i] == aParticle)
	//		{
	//			myParticles.erase(myParticles.begin() + i);
	//			break;
	//		}
	//	}
	//}
	//
	//void RenderScene::RemoveFromScene(Decal* aDecal)
	//{
	//	for (size_t i = 0; i < myDecals.size(); i++)
	//	{
	//		if (myDecals[i] == aDecal)
	//		{
	//			myDecals.erase(myDecals.begin() + i);
	//			break;
	//		}
	//	}
	//}
	//
	//void RenderScene::RemoveFromScene(SpotLight* aSpotLight)
	//{
	//	for (size_t i = 0; i < mySpotlights.size(); i++)
	//	{
	//		if (mySpotlights[i] == aSpotLight)
	//		{
	//			mySpotlights.erase(mySpotlights.begin() + i);
	//			break;
	//		}
	//	}
	//}
	//
	//void RenderScene::RemoveFromScene(SpriteInstance* aSprite)
	//{
	//	for (size_t i = 0; i < mySprites.size(); i++)
	//	{
	//		if (mySprites[i] == aSprite)
	//		{
	//			aSprite->RemoveFromScene();
	//			mySprites.erase(mySprites.begin() + i);
	//			break;
	//		}
	//	}
	//
	//}
	//
	//void RenderScene::RemoveFromScene(TextInstance* aText)
	//{
	//	for (size_t i = 0; i < myTexts.size(); i++)
	//	{
	//		if (myTexts[i] == aText)
	//		{
	//			myTexts[i]->SetAddedToSceneStatus(false);
	//			myTexts.erase(myTexts.begin() + i);
	//			return;
	//		}
	//	}
	//
	//	LOG_SYS_WARNING("Tried to remove invalid text :c");
	//}
	//
	//void RenderScene::RemoveAll()
	//{
	//	myModels.clear();
	//	for (auto& i : myParticles)
	//	{
	//		SAFE_DELETE(i);
	//	}
	//	myParticles.clear();
	//
	//	mySprites.clear();
	//
	//	myTexts.clear();
	//}
	//
	//void RenderScene::SetSkybox(ModelInstance* aSkybox)
	//{
	//	if (aSkybox->GetModelAsset().GetType() != ModelAsset::AssetType::SkyBox)
	//	{
	//		SYSERROR("SetSkybox with non skybox asset");
	//		return;
	//	}
	//	mySkybox = aSkybox;
	//}

	//void RenderScene::Imgui()
	//{
	//	WindowControl::Window("Scene", [this]()
	//	{
	//		for (ModelInstance* model : myModels)
	//		{
	//			ImGui::PushID(model);
	//			ImGui::Button("Model");
	//			if (ImGui::IsItemHovered())
	//			{
	//				model->SetIsHighlighted(true);
	//			}
	//			else
	//			{
	//				if (ImGui::IsWindowHovered())
	//				{
	//					model->SetIsHighlighted(false);
	//				}
	//			}
	//			ImGui::SameLine();
	//			ImGui::Text("%p", model);
	//			ImGui::PopID();
	//		}
	//	});
	//}
	//
	//std::vector<ParticleInstance*> RenderScene::GetParticles()
	//{
	//	return myParticles;
	//}

	std::vector<ModelInstance*> RenderScene::CullByFrustum(const tools::Frustum<float>& aFrustum)
	{
		thread_local std::vector<ModelInstance*> culledModels;
		culledModels.clear();
		culledModels.reserve(myModels.size());

		for (size_t i = 0; i < myModels.size(); ++i)
		{
			if (myModels[i]->ShouldRender() && aFrustum.Intersects(myModels[i]->GetBoundingSphere()))
			{
				culledModels.push_back(myModels[i]);
			}
		}

		return culledModels;
	}

	std::vector<ModelInstance*> RenderScene::Cull(const tools::Sphere<float>& aBoundingSphere)
	{
		std::vector<ModelInstance*> filtered;
		std::copy_if(myModels.begin(), myModels.end(), std::back_inserter(filtered), [&aBoundingSphere](ModelInstance* inst)
		{
			return aBoundingSphere.Intersects(inst->GetBoundingSphere());
		});
		return filtered;
	}
	/*
	std::vector<SpotLight*>& RenderScene::GetSpotLights()
	{
		return mySpotlights;
	}

	std::vector<Decal*>& RenderScene::GetDecals()
	{
		return myDecals;
	}
	*/

	std::array<PointLight*, NUMBEROFPOINTLIGHTS> RenderScene::CullPointLights(ModelInstance* aModel)
	{
		std::array<PointLight*, NUMBEROFPOINTLIGHTS> returnArr = { nullptr };

		int counter = 0;
		for (auto& light : myPointLights)
		{
			if (light->position.DistanceSqr(aModel->GetPosition()) < light->range * light->range)
			{
				returnArr[counter++] = light;

				if (counter == NUMBEROFPOINTLIGHTS)
				{
					break;
				}
			}
		}

		return returnArr;
	}

	/*
	const std::vector<SpriteInstance*>& RenderScene::GetSprites()
	{
		return mySprites;
	}

	const std::vector<TextInstance*>& RenderScene::GetText()
	{
		return myTexts;
	}

	void RenderScene::AddToScene(ParticleInstance* aParticle)
	{
		myParticles.push_back(aParticle);
	}
	*/

}