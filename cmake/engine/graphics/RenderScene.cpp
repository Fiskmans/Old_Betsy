#include "engine/graphics/RenderScene.h"

#include "engine/assets/PointLight.h"

#include "common/Macros.h"

#include "tools/Intersection.h"
#include "tools/Logger.h"

#include <algorithm>
#include <iterator>

namespace engine
{
	RenderScene::RenderScene(const std::string& aName)
		: ImGuiWindow("Scenes")
	{
		static int sceneIndex = 0;
		myName = aName + "##" + std::to_string(sceneIndex++);
	}

	void RenderScene::OnImgui()
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

	void RenderScene::AddToScene(ModelInstance* aModel)
	{
		myModels.push_back(aModel);
	}

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

	std::vector<ModelInstance*> RenderScene::Cull(const tools::Frustum<float>& aFrustum)
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

	std::array<PointLight*, NUMBEROFPOINTLIGHTS> RenderScene::FindAffectingPointLights(ModelInstance* aModel)
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
}