#pragma once
#include <Singleton.hpp>


typedef size_t(BufferConsumerFunction)(const char*, size_t);
enum AssetLoaderFunction : unsigned short
{
	AssetLoadetFunction_Texture,
	AssetLoaderFunction_Count
};

class AssetLoader : CommonUtilities::Singleton<AssetLoader>
{
public:
	AssetLoader();

	void LoadAssetFile(const std::string& aFilePath);
	void LoadAssetFolder(const std::string& aFolderPath, const std::string& aExtension = ".asset");

	void LoadAssetsFromMemory(const char* aData, size_t aDataSize);
private:


	size_t LoadAssetFromMemory(const char* aData,size_t aDataSize);


	size_t LoadTexture(const char* aData, size_t aDataSize);




	const std::array<std::function<BufferConsumerFunction>, static_cast<size_t>(AssetLoaderFunction_Count)> myLoaderFunctions;
};

