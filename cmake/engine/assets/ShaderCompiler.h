#ifndef ENGINE_ASSETS_SHADER_COMPILER_H
#define ENGINE_ASSETS_SHADER_COMPILER_H

#include "engine/assets/ShaderFlags.h"
#include "engine/assets/Asset.h"

#include <string>
#include <vector>

#include <d3d11.h>

namespace engine::assets
{
	class ShaderCompiler
	{
	public:
		ShaderCompiler(const std::string& aBakedFolderPath);

		Asset* GetPixelShader(const std::string& aBaseFolder, const std::string& aShader, ShaderFlags aFlags);
		Asset* GetVertexShader(const std::string& aBaseFolder, const std::string& aShader, ShaderFlags aFlags);
		Asset* GetGeometryShader(const std::string& aBaseFolder, const std::string& aShader, ShaderFlags aFlags);

		void ReloadShader(Asset* aAsset, const std::string& aBaseFolder, const std::string& aShader, ShaderFlags aFlags, const std::string& aFileChanged);

		void ForceRecompile();
		void DontForceRecompile();

	private:

		bool myForceRecompile = false;

		std::vector<char> LoadOrCompileFromFile(const std::string& aBBaseFolder, const std::string& aFilePath, const std::string& aEntryPoint, const std::string& aCompiler, ShaderFlags aFlags);

		std::string myBakedFolderPath;
	};

}

#endif