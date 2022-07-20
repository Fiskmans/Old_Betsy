
#include "engine/assets/AssetManager.h"

#include "tools/StringManipulation.h"

#include "logger/Logger.h"

//#include "LevelLoader.h"
//#include "NavMeshLoader.h"
//#include "AnimationLoader.h"

#include <filesystem>
#include <mutex>

#define TEXTURE_FOLDER "/textures/"
#define SKYBOX_FOLDER "/skyboxes/"
#define MODEL_FOLDER "/models/"
#define PIXELSHADER_FOLDER "/shaders/pixel/"
#define VERTEXSHADER_FOLDER "/shaders/vertex/"
#define GEOMETRYSHADER_FOLDER "/shaders/geometry/"
#define JSON_FOLDER "/json/"
#define FONT_FOLDER "/fonts/"
#define LEVEL_FOLDER "/levels/"
#define NAVMESH_FOLDER "/navmeshes/"
#define ANIMATIONS_FOLDER "/animations/"

#define BAKED_SHADER_FOLDER "/shaders/"

#define DEFAULT_PIXEL_SHADER "deferred/Deferred.hlsl"

namespace engine
{
    void AssetManager::Init(const std::string& aBaseFolder, const std::string& aBakeFolder)
    {
        myBaseFolder = aBaseFolder;
        //myModelLoader = new ModelLoader(aDevice, DEFAULT_PIXEL_SHADER);
        myTextureLoader = std::make_unique<assets::TextureLoader>();
        myShaderCompiler = std::make_unique<assets::ShaderCompiler>(aBakeFolder + BAKED_SHADER_FOLDER);

        //myErrorTexture = myTextureLoader->LoadTexture(aBaseFolder + TEXTURE_FOLDER + "engine/error.dds");
    }

    void AssetManager::Preload()
    {
        std::filesystem::path path(myBaseFolder + MODEL_FOLDER);

        if (path.is_relative())
        {
            path = std::filesystem::current_path() / path;
        }

        if (!std::filesystem::exists(path))
        {
            LOG_SYS_ERROR("Failed to perload assets, folder doesn't exist", path.string());
            return;
        }

        std::filesystem::recursive_directory_iterator iter = std::filesystem::recursive_directory_iterator(path);
        while (iter != std::filesystem::recursive_directory_iterator())
        {
            if (iter->path().extension() == ".fbx")
            {
                LOG_SYS_INFO("Preloading asset", iter->path().string());
                //GetModel(iter->path().string().substr((myBaseFolder + MODEL_FOLDER).length()));
            }
            iter++;
        }
    }

    void AssetManager::FlushChanges()
    {
        //myFileWatcher.FlushChanges();
    }

	AssetHandle AssetManager::GetTexture(const std::string& aPath, bool aFailSilent)
	{
		return GetTextureInternal(myBaseFolder + TEXTURE_FOLDER + aPath, aFailSilent);
	}

    //AssetHandle AssetManager::GetTextureRelative(const std::string& aBase, const std::string& aPath, bool aFailSilenty)
    //{
    //    return GetTextureInternal(tools::PathWithoutFile(aBase) + aPath, aFailSilenty);
    //}

    //AssetHandle AssetManager::GetCubeTexture(const std::string& aPath)
    //{
    //    if (myCachedCubeTextures.count(aPath) == 0)
    //    {
    //        Asset* texture = myTextureLoader->LoadCubeTexture(myBaseFolder + SKYBOX_FOLDER + aPath);
    //        if (!texture)
    //        {
    //            SYSERROR("Failed to load Cube Texture", aPath);
    //        }
    //        myCachedCubeTextures[aPath] = texture;
    //        return AssetHandle(texture);
    //    }
    //    return AssetHandle(myCachedCubeTextures[aPath]);
    //}
    //
    //AssetHandle AssetManager::GetModel(const std::string& aPath)
    //{
    //    if (myCachedModels.count(aPath) == 0)
    //    {
    //        Asset* model = myModelLoader->LoadModel(myBaseFolder + MODEL_FOLDER + aPath);
    //        if (!model)
    //        {
    //            SYSERROR("Failed to load Model", aPath);
    //        }
    //        myCachedModels[aPath] = model;
    //        return AssetHandle(model);
    //    }
    //    return AssetHandle(myCachedModels[aPath]);
    //}
    //
    //AssetHandle AssetManager::GetSkybox(const std::string& aPath)
    //{
    //    if (myCachedSkyboxes.count(aPath) == 0)
    //    {
    //        Asset* model = myModelLoader->LoadSkybox(aPath);
    //        if (!model)
    //        {
    //            SYSERROR("Failed to load skybox", aPath);
    //        }
    //        myCachedSkyboxes[aPath] = model;
    //        return AssetHandle(model);
    //    }
    //    return AssetHandle(myCachedSkyboxes[aPath]);
    //}

    AssetHandle AssetManager::GetPixelShader(const std::string& aPath, ShaderFlags aFlags)
    {
        static std::mutex mutex;
        std::lock_guard lock(mutex);

        if (myCachedPixelShaders[aPath].count(aFlags) == 0)
        {
            Asset* shader = myShaderCompiler->GetPixelShader(myBaseFolder + PIXELSHADER_FOLDER, aPath, aFlags);
            if (!shader)
            {
                LOG_SYS_ERROR("Failed to load pixelShader", aPath);
            }
            myCachedPixelShaders[aPath][aFlags] = shader;

            /*
            Tools::FileWatcher::CallbackFunction callback = std::bind(
                &ShaderCompiler::ReloadShader,
                myShaderCompiler,
                shader,
                myBaseFolder + PIXELSHADER_FOLDER,
                aPath,
                aFlags,
                std::placeholders::_1);

            shader->myFileHandle = myFileWatcher.RegisterCallback(myBaseFolder + PIXELSHADER_FOLDER + aPath, callback);
            */

            return AssetHandle(shader);
        }

        return AssetHandle(myCachedPixelShaders[aPath][aFlags]);
    }

    AssetHandle AssetManager::GetVertexShader(const std::string& aPath, ShaderFlags aFlags)
    {
        static std::mutex mutex;
        std::lock_guard lock(mutex);

        if (myCachedVertexShaders[aPath].count(aFlags) == 0)
        {
            Asset* shader = myShaderCompiler->GetVertexShader(myBaseFolder + VERTEXSHADER_FOLDER, aPath, aFlags);
            if (!shader)
            {
                LOG_SYS_ERROR("Failed to load pixelShader", aPath);
            }

            myCachedVertexShaders[aPath][aFlags] = shader;

            /*
            tools::FileWatcher::CallbackFunction callback = std::bind(
                &ShaderCompiler::ReloadShader,
                myShaderCompiler,
                shader,
                myBaseFolder + VERTEXSHADER_FOLDER,
                aPath,
                aFlags,
                std::placeholders::_1);

            shader->myFileHandle = myFileWatcher.RegisterCallback(myBaseFolder + VERTEXSHADER_FOLDER + aPath, callback);
            */

            return AssetHandle(shader);
        }

        return AssetHandle(myCachedVertexShaders[aPath][aFlags]);
    }

    //AssetHandle AssetManager::GetGeometryShader(const std::string& aPath, ShaderFlags aFlags)
    //{
    //    static std::mutex mutex;
    //    std::lock_guard lock(mutex);
    //
    //    if (myCachedGeometryShaders[aPath].count(aFlags) == 0)
    //    {
    //        Asset* shader = myShaderCompiler->GetGeometryShader(myBaseFolder + GEOMETRYSHADER_FOLDER, aPath, aFlags);
    //        if (!shader)
    //        {
    //            SYSERROR("Failed to load pixelShader", aPath);
    //        }
    //
    //        myCachedGeometryShaders[aPath][aFlags] = shader;
    //
    //#if USEFILEWATHCER
    //        Tools::FileWatcher::CallbackFunction callback = std::bind(
    //            &ShaderCompiler::ReloadShader,
    //            myShaderCompiler,
    //            shader,
    //            myBaseFolder + GEOMETRYSHADER_FOLDER,
    //            aPath,
    //            aFlags,
    //            std::placeholders::_1);
    //
    //        shader->myFileHandle = myFileWatcher.RegisterCallback(myBaseFolder + GEOMETRYSHADER_FOLDER + aPath, callback);
    //#endif
    //        return AssetHandle(shader);
    //    }
    //
    //    return AssetHandle(myCachedGeometryShaders[aPath][aFlags]);
    //}
    //
    //AssetHandle AssetManager::GetPerlinTexture(V2ui aSize, V2f aScale, unsigned int aSeed)
    //{
    //    Asset* texture = myTextureLoader->GeneratePerlin(aSize, aScale, aSeed);
    //    AssetHandle out = AssetHandle(texture);
    //    myCustomAssets.push_back(texture);
    //    return out;
    //}
    //
    //AssetHandle AssetManager::GetJSON(const std::string& aPath)
    //{
    //    return GetJSONInternal(myBaseFolder + JSON_FOLDER + aPath);
    //}
    //
    //AssetHandle AssetManager::GetJSONRelative(const std::string& aBase, const std::string& aPath)
    //{
    //    return GetJSONInternal(Tools::PathWithoutFile(aBase) + aPath);
    //}
    //
    //AssetHandle AssetManager::GetFont(const std::string& aPath)
    //{
    //    if (myCachedfonts.count(aPath) == 0)
    //    {
    //        std::string fullPath = myBaseFolder + FONT_FOLDER + aPath;
    //        Asset* font = new FontAsset(new DirectX::SpriteFont(myDevice, (std::wstring(fullPath.begin(), fullPath.end())).c_str(), true));
    //        if (!font)
    //        {
    //            SYSERROR("Failed to load font", aPath);
    //        }
    //        myCachedfonts[aPath] = font;
    //        return AssetHandle(font);
    //    }
    //    return AssetHandle(myCachedfonts[aPath]);
    //}
    //
    //AssetHandle AssetManager::GetLevel(const std::string& aPath)
    //{
    //    if (myCachedLevels.count(aPath) == 0)
    //    {
    //        Asset* level = LevelLoader::LoadLevel(myBaseFolder + LEVEL_FOLDER + aPath);
    //        if (!level)
    //        {
    //            SYSERROR("Failed to start loading level", aPath);
    //        }
    //        myCachedLevels[aPath] = level;
    //        return AssetHandle(level);
    //    }
    //    return AssetHandle(myCachedLevels[aPath]);
    //}
    //
    //AssetHandle AssetManager::GetNavMesh(const std::string& aPath)
    //{
    //    if (myCachedNavMeshes.count(aPath) == 0)
    //    {
    //        Asset* navmesh = NavMeshLoader::LoadNavMesh(myBaseFolder + NAVMESH_FOLDER + aPath);
    //        if (!navmesh)
    //        {
    //            SYSERROR("Failed to load navmesh", aPath);
    //        }
    //        myCachedNavMeshes[aPath] = navmesh;
    //        return AssetHandle(navmesh);
    //    }
    //    return AssetHandle(myCachedNavMeshes[aPath]);
    //}
    //
    //AssetHandle AssetManager::GetAnimation(const std::string& aPath)
    //{
    //    return GetAnimationInternal(myBaseFolder + ANIMATIONS_FOLDER + aPath);
    //}
    //
    //AssetHandle AssetManager::GetAnimationRelative(const std::string& aBase, const std::string& aPath)
    //{
    //    return GetAnimationInternal(Tools::PathWithoutFile(aBase) + aPath);
    //}

    void AssetManager::AssumeOwnershipOfCustomAsset(Asset* aCustomAsset)
    {
        myCustomAssets.push_back(aCustomAsset);
    }

    //void AssetManager::ImGui()
    //{
    //    WindowControl::Window("AssetManager", [this]()
    //        {
    //            if (ImGui::TreeNode("Shaders"))
    //            {
    //                ImGui::Button("Reload", ImVec2(60, 40));
    //                if (ImGui::BeginDragDropTarget())
    //                {
    //                    const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Select_Pixelshader");
    //                    if (payload)
    //                    {
    //                        std::string path = std::string(reinterpret_cast<const char*>(payload->Data), payload->DataSize);
    //                    
    //                        myShaderCompiler->ForceRecompile();
    //                        if (myCachedPixelShaders.count(path) != 0)
    //                        {
    //                            for (auto& i : myCachedPixelShaders[path])
    //                            {
    //                                myFileWatcher.Trigger(i.second->myFileHandle);
    //                            }
    //                        }
    //                        myShaderCompiler->DontForceRecompile();
    //                    }
    //
    //                    ImGui::EndDragDropTarget();
    //                }
    //
    //                for (auto& ShaderList : myCachedPixelShaders)
    //                {
    //                    ImGui::Button(ShaderList.first.c_str());
    //                    if (ImGui::BeginDragDropSource())
    //                    {
    //                        ImGui::SetDragDropPayload("Select_Pixelshader", ShaderList.first.c_str(), ShaderList.first.length(), ImGuiCond_Once);
    //
    //                        ImGui::Text(ShaderList.first.c_str());
    //
    //                        ImGui::EndDragDropSource();
    //                    }
    //                }
    //
    //                ImGui::TreePop();
    //            }
    //
    //            if (ImGui::TreeNode("Models"))
    //            {
    //                for (auto& model : myCachedModels)
    //                {
    //                    ImGui::Button(model.first.c_str());
    //                    if (ImGui::BeginDragDropSource())
    //                    {
    //                        ImGui::SetDragDropPayload("Asset", &model.second, sizeof(Asset*), ImGuiCond_Once);
    //
    //                        ImGui::Text(model.first.c_str());
    //
    //                        ImGui::EndDragDropSource();
    //                    }
    //                }
    //                ImGui::TreePop();
    //            }
    //        });
    //}
    
	AssetHandle AssetManager::GetTextureInternal(const std::string& aPath, bool aFailSilenty)
	{
		if (myCachedTextures.count(aPath) == 0)
		{
			Asset* texture = myTextureLoader->LoadTexture(aPath, aFailSilenty);
			if (!texture)
			{
				if (!aFailSilenty)
				{
					LOG_ERROR("Failed to load Texture", aPath);
				}
				return AssetHandle(myErrorTexture);
			}
			myCachedTextures[aPath] = texture;
			return AssetHandle(texture);
		}
		return AssetHandle(myCachedTextures[aPath]);
	}

    //AssetHandle AssetManager::GetJSONInternal(const std::string& aPath)
    //{
    //    if (myCachedJSON.count(aPath) == 0)
    //    {
    //        Asset* json = nullptr;
    //        FiskJSON::Object* obj = new FiskJSON::Object();
    //        try
    //        {
    //            obj->Parse(Tools::ReadWholeFile(aPath));
    //            json = new JSONAsset(obj,aPath);
    //        }
    //        catch (const std::exception& e)
    //        {
    //            SYSERROR("Failed to load json", aPath, e.what());
    //
    //            delete obj;
    //        }
    //
    //        myCachedJSON[aPath] = json;
    //        return AssetHandle(json);
    //    }
    //    return AssetHandle(myCachedJSON[aPath]);
    //}
    //
    //AssetHandle AssetManager::GetAnimationInternal(const std::string& aPath)
    //{
    //    if (myCachedAnimations.count(aPath) == 0)
    //    {
    //        Asset* model = AnimationLoader::BakeAnimation(aPath);
    //        myCachedAnimations[aPath] = model;
    //        return AssetHandle(model);
    //    }
    //    return AssetHandle(myCachedAnimations[aPath]);
    //}
}