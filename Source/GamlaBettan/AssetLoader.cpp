#include "pch.h"
#include "AssetLoader.h"


#pragma pack(push,1)
struct AssetFileHeader
{
	char marker[16] = "FiskAssetLoader";
	size_t AssetLoaderVersion;
	unsigned short AssetCount;
	char reserved[102];
};

struct CommonAssetHeader
{
	AssetLoaderFunction functionIndex;
	size_t assetSize;
	char reserved[6];
};
#pragma pack(pop)

#define ASSETFILEMINIMUMFILESIZE (sizeof(AssetFileHeader) + sizeof(CommonAssetHeader))
#define ASSETLOADERVERSION (1ULL)
#define BINDLOADER(name) (std::bind(&AssetLoader::##name,this,std::placeholders::_1,std::placeholders::_2))

AssetLoader::AssetLoader() : 
	myLoaderFunctions
	{
		BINDLOADER(LoadTexture)
	}
{
}

void AssetLoader::LoadAssetFile(const std::string& aFilePath)
{
	static_assert(sizeof(AssetFileHeader) == 128,"size of assetfile header doesnt equal 128");
	std::string data = Tools::ReadWholeFile(aFilePath);
	if (data.size() > ASSETFILEMINIMUMFILESIZE)
	{
		LoadAssetsFromMemory(data.c_str(),data.length());
	}
	else
	{
		SYSERROR("Asset file is not large enough to contain everything neaded to load a file, file is corrupt or empty", aFilePath);
	}
}

void AssetLoader::LoadAssetsFromMemory(const char* aData, size_t aDataSize)
{
	static_assert(sizeof(CommonAssetHeader) == 16, "size of common asset header is not 16");

	if (aDataSize - sizeof(AssetFileHeader) > sizeof(CommonAssetHeader))
	{
		const AssetFileHeader* header = reinterpret_cast<const AssetFileHeader*>(aData);
		if (header->AssetLoaderVersion > ASSETLOADERVERSION)
		{
			SYSERROR("Assetfile is not compatible with this version of the loader","");
			return;
		}
		if (header->AssetLoaderVersion < ASSETLOADERVERSION)
		{
			SYSWARNING("Assetfile was created using an older version of the assetloader using legacyloader", "");
			SYSERROR("LegacyLoader has not been implemented yet recreate the asset using the correct vesrion", "");
			return;
		}

		size_t dataLeft = aDataSize;
		const char* readingHead = aData;

		if (dataLeft >= sizeof(AssetFileHeader))
		{
			dataLeft -= sizeof(AssetFileHeader);
			readingHead += sizeof(AssetFileHeader);
		}
		else
		{
			SYSERROR("Asset File is corrupt", "");
			return;
		}

		for (size_t i = 0; i < header->AssetCount; i++)
		{
			size_t read = LoadAssetFromMemory(readingHead, dataLeft);
			if (dataLeft >= read)	
			{														
				dataLeft -= read;								
				readingHead += read;								
			}														
			else													
			{														
				SYSERROR("Asset File is corrupt", "");				
				return;												
			}
		}

	}
	else
	{
		SYSERROR("Asset file is not large enough to contain a common header","");
	}
}

size_t AssetLoader::LoadAssetFromMemory(const char* aData, size_t aDataSize)
{
	size_t read = 0;
	const CommonAssetHeader* assetHeader = reinterpret_cast<const CommonAssetHeader*>(aData);
	if (assetHeader->functionIndex < AssetLoaderFunction_Count)
	{
		read += myLoaderFunctions[assetHeader->functionIndex](aData + sizeof(CommonAssetHeader), aDataSize - sizeof(CommonAssetHeader));
	}
	else
	{
		SYSERROR("Asset File is corrupt", "");
	}

	return sizeof(CommonAssetHeader) + read;
}

size_t AssetLoader::LoadTexture(const char* aData, size_t aDataSize)
{
	

	return size_t();
}


#undef ASSETFILEMINIMUMFILESIZE
#undef ASSETLOADERVERSION
#undef BINDLOADER