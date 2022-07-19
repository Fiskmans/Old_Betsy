#ifndef ENGINE_ASSETS_TEXTURE_LOADER_H
#define ENGINE_ASSETS_TEXTURE_LOADER_H

#include "engine/assets/Asset.h"

#include "tools/MathVector.h"

namespace engine::assets
{
	class TextureLoader
	{
	public:
		Asset* LoadTexture(const std::string& aFile, bool aFailSilenty = false);
		Asset* LoadCubeTexture(const std::string& aFile);
		//Asset* GeneratePerlin(V2ui aSize, V2f aScale = { 1.f, 1.f }, unsigned int aSeed = 0);

	private:
		Asset* LoadTextureInternal(const std::string& aFile, size_t aFlags, bool aFailSilenty);
	};
}

#endif