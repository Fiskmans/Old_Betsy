#pragma once
#include <string>
#include "ShaderFlags.h"
#include <vector>

struct ID3D11Device;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11GeometryShader;

class ShaderCompiler
{
public:
	ShaderCompiler(ID3D11Device* aDevice, const std::string& aBakedFolderPath);

	Asset* GetPixelShader(const std::string& aBaseFolder, const std::string& aShader, ShaderFlags aFlags);
	Asset* GetVertexShader(const std::string& aBaseFolder, const std::string& aShader, ShaderFlags aFlags);
	Asset* GetGeometryShader(const std::string& aBaseFolder, const std::string& aShader, ShaderFlags aFlags);

	void ReloadShader(Asset* aAsset, const std::string& aBaseFolder, const std::string& aShader, ShaderFlags aFlags, const std::string& aFileChanged);

private:
	std::vector<char> LoadOrCompileFromFile(const std::string& aBBaseFolder, const std::string& aFilePath, const std::string& aEntryPoint, const std::string& aCompiler, ShaderFlags aFlags);

	ID3D11Device* myDevice;
	std::string myBakedFolderPath;
};