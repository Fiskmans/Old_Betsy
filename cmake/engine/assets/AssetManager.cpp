#include "engine/assets/AssetManager.h"

#include "engine/graphics/TextureFactory.h"

#include "tools/FileHelpers.h"
#include "tools/JSON.h"
#include "tools/Logger.h"
#include "tools/StringManipulation.h"

#include "imgui/WindowControl.h"

#include <filesystem>
#include <mutex>

#define TEXTURE_FOLDER "textures/"
#define SKYBOX_FOLDER "skyboxes/"
#define MODEL_FOLDER "models/"

#define SHADER_BASE "shaders/"

#define PIXELSHADER_FOLDER "pixel/"
#define VERTEXSHADER_FOLDER "vertex/"
#define GEOMETRYSHADER_FOLDER "geometry/"

#define JSON_FOLDER "json/"
#define FONT_FOLDER "fonts/"
#define LEVEL_FOLDER "levels/"
#define NAVMESH_FOLDER "navmeshes/"
#define ANIMATIONS_FOLDER "animations/"

#define BAKED_SHADER_FOLDER "shaders/"

#define DEFAULT_PIXEL_SHADER "deferred/Deferred.hlsl"

namespace engine
{
    AssetManager::AssetManager()
        : myCustomTextureCounter(0)
    {
    }
    void AssetManager::Init(const std::string& aBaseFolder, const std::string& aBakeFolder)
    {
        assert(aBaseFolder.length() > 0);
		assert(aBaseFolder.at(aBaseFolder.length() - 1) == '/');

		assert(aBakeFolder.length() > 0);
		assert(aBakeFolder.at(aBakeFolder.length() - 1) == '/');

        myBaseFolder = aBaseFolder;
        myModelLoader = std::make_unique<assets::ModelLoader>(DEFAULT_PIXEL_SHADER);
        myTextureLoader = std::make_unique<assets::TextureLoader>();
        myShaderCompiler = std::make_unique<assets::ShaderCompiler>(aBakeFolder + BAKED_SHADER_FOLDER);

        if (::GetAsyncKeyState(VK_SHIFT) && ::GetAsyncKeyState(VK_SHIFT))
            myShaderCompiler->ForceRecompile();
    }

    void AssetManager::Preload()
    {
        std::filesystem::path rootPath(myBaseFolder + MODEL_FOLDER);

        if (rootPath.is_relative())
        {
            rootPath = std::filesystem::current_path() / rootPath;
        }

        if (!std::filesystem::exists(rootPath))
        {
            LOG_SYS_ERROR("Failed to preload assets, root folder doesn't exist", rootPath.string());
            return;
        }

        std::filesystem::recursive_directory_iterator iter = std::filesystem::recursive_directory_iterator(rootPath);
        while (iter != std::filesystem::recursive_directory_iterator())
        {
            if (iter->path().extension() == ".fbx")
            {
                LOG_SYS_INFO("Preloading asset", iter->path().string());
                GetModel(tools::NormalizeSlashes(iter->path().string().substr(rootPath.string().length())));
            }
            iter++;
        }
    }

	AssetHandle<TextureAsset> AssetManager::GetTexture(const std::string& aPath, bool aFailSilent)
	{
		return GetTextureInternal(myBaseFolder + TEXTURE_FOLDER + aPath, aFailSilent);
	}

    AssetHandle<DrawableTextureAsset> AssetManager::MakeTexture(const tools::V2ui& aResolution, DXGI_FORMAT aFormat)
    {
        return new DrawableTextureAsset(graphics::TextureFactory::GetInstance().CreateTexture(aResolution, aFormat, "CustomTexture" + std::to_string(myCustomTextureCounter++)));
    }

    AssetHandle<DepthTextureAsset> AssetManager::MakeDepthTexture(const tools::V2ui& aResolution)
    {
        return new DepthTextureAsset(graphics::TextureFactory::GetInstance().CreateDepth(aResolution, "Depth"));
    }

    AssetHandle<GBufferAsset> AssetManager::MakeGBuffer(const tools::V2ui& aResolution)
    {
        return new GBufferAsset(graphics::TextureFactory::GetInstance().CreateGBuffer(aResolution, "GBuffer"));
    }

    AssetHandle<TextureAsset> AssetManager::GetTextureRelative(const std::string& aBase, const std::string& aPath, bool aFailSilenty)
    {
        return GetTextureInternal(tools::PathWithoutFile(aBase) + aPath, aFailSilenty);
    }

	AssetHandle<TextureAsset> AssetManager::GetCubeTexture(const std::string& aPath)
	{
		if (myCachedCubeTextures.count(aPath) == 0)
		{
			Asset* texture = myTextureLoader->LoadCubeTexture(myBaseFolder + SKYBOX_FOLDER + aPath);
			if (!texture)
			{
				LOG_SYS_ERROR("Failed to load Cube Texture", aPath);
			}
			myCachedCubeTextures[aPath] = texture;
			return texture;
		}
		return myCachedCubeTextures[aPath];
	}

	AssetHandle<ModelAsset> AssetManager::GetModel(const std::string& aPath)
	{
		if (myCachedModels.count(aPath) == 0)
		{
			Asset* model = myModelLoader->LoadModel(myBaseFolder + MODEL_FOLDER + aPath);
			if (!model)
			{
				LOG_ERROR("Failed to load Model", aPath);
			}
			myCachedModels[aPath] = model;
			return model;
		}
		return myCachedModels[aPath];
	}

    AssetHandle<PixelShaderAsset> AssetManager::GetPixelShader(const std::string& aPath, ShaderFlags aFlags)
    {
        static std::mutex mutex;
        std::lock_guard lock(mutex);

        if (myCachedPixelShaders[aPath].count(aFlags) == 0)
        {
            Asset* shader = myShaderCompiler->GetPixelShader(myBaseFolder + SHADER_BASE, PIXELSHADER_FOLDER + aPath, aFlags);
            if (!shader)
            {
                LOG_SYS_ERROR("Failed to load pixelShader", aPath);
            }
            myCachedPixelShaders[aPath][aFlags] = shader;

            return shader;
        }

        return myCachedPixelShaders[aPath][aFlags];
    }

    AssetHandle<VertexShaderAsset> AssetManager::GetVertexShader(const std::string& aPath, ShaderFlags aFlags)
    {
        static std::mutex mutex;
        std::lock_guard lock(mutex);

        if (myCachedVertexShaders[aPath].count(aFlags) == 0)
        {
            Asset* shader = myShaderCompiler->GetVertexShader(myBaseFolder + SHADER_BASE, VERTEXSHADER_FOLDER + aPath, aFlags);
            if (!shader)
            {
                LOG_SYS_ERROR("Failed to load pixelShader", aPath);
            }

            myCachedVertexShaders[aPath][aFlags] = shader;

            return shader;
        }

        return myCachedVertexShaders[aPath][aFlags];
    }

    AssetHandle<JsonAsset> AssetManager::GetJSONRelative(const std::string& aBase, const std::string& aPath)
    {
        return GetJSONInternal(tools::PathWithoutFile(aBase) + aPath);
    }
    
    void AssetManager::AssumeOwnershipOfCustomAsset(Asset* aCustomAsset)
    {
        myCustomAssets.push_back(aCustomAsset);
    }

    void AssetManager::ImGui()
    {
        old_betsy_imgui::WindowControl::Window("AssetManager", [this]() { Imguicontent(); });
    }

    void AssetManager::Imguicontent()
	{
		if (ImGui::TreeNode("Shaders"))
		{
			ImGui::Button("Reload", ImVec2(60, 40));

			for (auto& ShaderList : myCachedPixelShaders)
			{
				ImGui::Button(ShaderList.first.c_str());
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Models"))
		{
			for (auto& model : myCachedModels)
			{
				ImGui::Button(model.first.c_str());
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("Asset", &model.second, sizeof(Asset*), ImGuiCond_Once);

					ImGui::Text("%s", model.first.c_str());

					ImGui::EndDragDropSource();
				}
			}
			ImGui::TreePop();
		}
    }
    
	AssetHandle<TextureAsset> AssetManager::GetTextureInternal(const std::string& aPath, bool aFailSilenty)
	{
		if (myCachedTextures.count(aPath) == 0)
		{
			Asset* texture = myTextureLoader->LoadTexture(aPath, aFailSilenty);
			if (!texture)
			{
				if (!aFailSilenty)
					LOG_ERROR("Failed to load Texture", aPath);
				
                return myErrorTexture;
			}
			myCachedTextures[aPath] = texture;
			return texture;
		}
		return myCachedTextures[aPath];
	}

	AssetHandle<JsonAsset> AssetManager::GetJSONInternal(const std::string& aPath)
	{
		if (myCachedJSON.count(aPath) == 0)
		{
			Asset* json = nullptr;
			json = new JsonAsset(aPath);
			myCachedJSON[aPath] = json;

			return json;
		}
		return myCachedJSON[aPath];
	}
}