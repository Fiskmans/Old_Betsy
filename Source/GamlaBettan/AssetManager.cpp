#include "pch.h"
#include "AssetManager.h"

AssetHandle AssetManager::GetModel(const std::string& aPath)
{
    if (myCachedModels.count(aPath) == 0)
    {
        Asset* model = myModelLoader->LoadModel(aPath);
        if (!model)
        {
            SYSERROR("Failed to loadModel",aPath);
        }
        myCachedModels[aPath] = model;
        return AssetHandle(model);
    }
    return AssetHandle(myCachedModels[aPath]);
}

AssetHandle AssetManager::GetSkybox(const std::string& aPath)
{
    if (myCachedSkyboxes.count(aPath) == 0)
    {
        Asset* model = myModelLoader->LoadSkybox(aPath);
        if (!model)
        {
            SYSERROR("Failed to skybox", aPath);
        }
        myCachedSkyboxes[aPath] = model;
        return AssetHandle(model);
    }
    return AssetHandle(myCachedSkyboxes[aPath]);
}
