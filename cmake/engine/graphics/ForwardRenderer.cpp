#include "engine/graphics/ForwardRenderer.h"

#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/RenderManager.h"
#include "engine/graphics/ShaderBuffers.h"
#include "engine/graphics/ShaderMappings.h"

#include "engine/assets/Model.h"

#include <algorithm>

namespace engine::graphics
{
	bool ForwardRenderer::Init()
	{
		if (!RenderManager::GetInstance().CreateGenericShaderBuffer(myObjectBuffer, sizeof(ObjectBuffer)))
		{
			LOG_SYS_ERROR("Failed to create forward renderers object buffer");
			return false;
		}

		return true;
	}
	
	void ForwardRenderer::Render(std::vector<ModelInstance*>& aModels, Camera& aCamera)
	{
		ID3D11DeviceContext* context = GraphicsEngine::GetInstance().GetFrameWork().GetContext();

		std::sort(aModels.begin(), aModels.end(), [&aCamera](ModelInstance* aLeft, ModelInstance* aRight)
		{
			float leftDistance = tools::V3f(aLeft->GetPosition()).Dot(aCamera.GetForward());
			float rightDistance = tools::V3f(aRight->GetPosition()).Dot(aCamera.GetForward());
			return leftDistance < rightDistance;
		});

		for (ModelInstance* modelInstance : aModels)
		{
			const engine::Model* rawModel = modelInstance->GetModelAsset().myModel;
		


			
			for (const engine::Model::ModelData* modelData : rawModel->GetModelData())
			{

				ObjectBuffer oBuffer;
				oBuffer.myDiffuseColor = modelData->myDiffuseColor;
				oBuffer.myModelToWorldSpace = modelInstance->GetModelToWorldTransform().Transposed();
				oBuffer.myObjectId = modelInstance->GetId();
				oBuffer.myObjectLifeTime = (Time::Now() - modelInstance->GetSpawnTime()).count();

				RenderManager::GetInstance().OverWriteBuffer(myObjectBuffer, &oBuffer, sizeof(oBuffer));

				context->PSSetConstantBuffers(shader_mappings::BUFFER_OBJECT, 1, &myObjectBuffer);
				context->VSSetConstantBuffers(shader_mappings::BUFFER_OBJECT, 1, &myObjectBuffer);

				context->PSSetShader(modelData->myPixelShader.Access().myShader, nullptr, 0);
				context->VSSetShader(modelData->myVertexShader.Access().myShader, nullptr, 0);

				UINT vertexOffset = 0;

				context->IASetVertexBuffers(0, 1, &modelData->myVertexBuffer, &modelData->myStride, &vertexOffset);

				if (modelData->myIsIndexed)
				{
					context->IASetIndexBuffer(modelData->myIndexBuffer, modelData->myIndexBufferFormat, 0);
					context->DrawIndexed(modelData->myNumberOfIndexes, 0, 0);
				}
				else
				{
					context->Draw(modelData->myNumberOfVertexes, 0);
				}

			}
		}
	}
}
