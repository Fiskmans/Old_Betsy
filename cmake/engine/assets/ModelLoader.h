#ifndef ENGINE_GRAPHICS_MODEL_LOADER_H
#define ENGINE_GRAPHICS_MODEL_LOADER_H

#include "engine/assets/Asset.h"

#include "tools/MathVector.h"

#include <vector>
#include <thread>
#include <atomic>
#include <string>
#include <unordered_map>

#include <assimp/scene.h>

namespace engine::assets
{

	class ModelLoader
	{
	public:
		ModelLoader(const std::string& aDefaultPixelShader);
		~ModelLoader();

		[[nodiscard]] Asset* LoadModel(const std::string& aFilePath);

	private:
		void PrepareModel(Model* aModel, const std::string& aPath);

		void LoadLoop();

		void LoadModel_Internal(Model* aModel, const std::string& aFilePath);

		void LoadNode(const aiScene* aScene, const aiNode* aNode, aiMatrix4x4 aTransform, Model* aModel, std::unordered_map<std::string, std::string>& aInOutAttributes, const std::string& aFilePath);
		void LoadMesh(const aiScene* aScene, const aiNode* aNode, aiMatrix4x4 aTransform, const aiMesh* aMesh, Model* aModel, std::unordered_map<std::string, std::string>& aInOutAttributes, const std::string& aFilePath);
		void LoadAttributes(const aiNode* aNode, const aiMaterial* aMaterial, std::unordered_map<std::string, std::string>& aInOutAttributes, std::unordered_map<std::string, tools::V3f>& aInOutColors);

		void QueueLoad(Model* aModel, std::string aFilePath);


		static const size_t myHandoverSlots = 40;
		struct LoadRequest
		{
			Model* myModel = nullptr;
			std::string myFilePath = "";
			std::atomic<bool> myEmpty = true;

			LoadRequest& operator=(const LoadRequest& another)
			{
				myModel = another.myModel;
				myFilePath = another.myFilePath;
				myEmpty = another.myEmpty.load();

				return (*this);
			}

		} myHandovers[myHandoverSlots];


		std::string myDefaultPixelShader;
		std::atomic<bool> myIsRunning = true;

		std::thread myWorkHorse;
	};

}

#endif