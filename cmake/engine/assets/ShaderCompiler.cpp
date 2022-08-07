
#include "engine/assets/ShaderCompiler.h"
#include "engine/assets/ShaderTypes.h"

#include "engine/graphics/GraphicEngine.h"

#include "logger/Logger.h"

#include "tools/FileHelpers.h"

#include <d3dcompiler.h>

#include <fstream>
#include <unordered_map>
#include <filesystem>



#define SHADER_VERSION (2)
namespace engine::assets
{
	ShaderCompiler::ShaderCompiler(const std::string& aBakedFolderPath)
	{
		myBakedFolderPath = aBakedFolderPath;
	}

	Asset* ShaderCompiler::GetPixelShader(const std::string& aBaseFolder, const std::string& aShader, ShaderFlags aFlags)
	{
		std::vector<char> blob = LoadOrCompileFromFile(aBaseFolder, aShader, "pixelShader", "ps_5_0", aFlags);

		ID3D11PixelShader* shader;
		HRESULT result = graphics::GraphicsEngine::GetInstance().GetFrameWork().GetDevice()->CreatePixelShader(blob.data(), blob.size(), nullptr, &shader);
		if (FAILED(result))
		{
			return nullptr;
		}

		return new PixelShaderAsset(shader);
	}

	Asset* ShaderCompiler::GetVertexShader(const std::string& aBaseFolder, const std::string& aShader, ShaderFlags aFlags)
	{
		std::vector<char> blob = LoadOrCompileFromFile(aBaseFolder, aShader, "vertexShader", "vs_5_0", aFlags);

		ID3D11VertexShader* shader;
		HRESULT result = graphics::GraphicsEngine::GetInstance().GetFrameWork().GetDevice()->CreateVertexShader(blob.data(), blob.size(), nullptr, &shader);
		if (FAILED(result))
		{
			return nullptr;
		}

		return new VertexShaderAsset(shader, blob);
	}

	Asset* ShaderCompiler::GetGeometryShader(const std::string& aBaseFolder, const std::string& aShader, ShaderFlags aFlags)
	{
		std::vector<char> blob = LoadOrCompileFromFile(aBaseFolder, aShader, "geometryShader", "gs_5_0", aFlags);

		ID3D11GeometryShader* shader;
		HRESULT result = graphics::GraphicsEngine::GetInstance().GetFrameWork().GetDevice()->CreateGeometryShader(blob.data(), blob.size(), nullptr, &shader);
		if (FAILED(result))
		{
			return nullptr;
		}

		return new GeometryShaderAsset(shader);
	}

	void ShaderCompiler::ReloadShader(Asset* aAsset, const std::string& aBaseFolder, const std::string& aShader, ShaderFlags aFlags, const std::string& aFileChanged)
	{
		LOG_SYS_INFO("Reloading shader", aFileChanged);

		Asset* newShader;
	
		if (aAsset->Is<PixelShaderAsset>())
		{
			newShader = GetPixelShader(aBaseFolder, aShader, aFlags);
			if (newShader)
			{
				reinterpret_cast<PixelShaderAsset*>(aAsset)->myShader->Release();
				reinterpret_cast<PixelShaderAsset*>(aAsset)->myShader = reinterpret_cast<PixelShaderAsset*>(newShader)->myShader;
				reinterpret_cast<PixelShaderAsset*>(newShader)->myShader = nullptr;
				delete newShader;
			}
		}
		else if (aAsset->Is<VertexShaderAsset>())
		{
			newShader = GetVertexShader(aBaseFolder, aShader, aFlags);
			if (newShader)
			{
				reinterpret_cast<VertexShaderAsset*>(aAsset)->myShader->Release();
				reinterpret_cast<VertexShaderAsset*>(aAsset)->myShader = reinterpret_cast<VertexShaderAsset*>(newShader)->myShader;
				reinterpret_cast<VertexShaderAsset*>(aAsset)->myBlob = reinterpret_cast<VertexShaderAsset*>(newShader)->myBlob;
				reinterpret_cast<VertexShaderAsset*>(newShader)->myShader = nullptr;
				delete newShader;
			}
		}
		else if (aAsset->Is<GeometryShaderAsset>())
		{
			newShader = GetGeometryShader(aBaseFolder, aShader, aFlags);
			if (newShader)
			{
				reinterpret_cast<GeometryShaderAsset*>(aAsset)->myShader->Release();
				reinterpret_cast<GeometryShaderAsset*>(aAsset)->myShader = reinterpret_cast<GeometryShaderAsset*>(newShader)->myShader;
				reinterpret_cast<GeometryShaderAsset*>(newShader)->myShader = nullptr;
				delete newShader;
			}
		}
		else
		{
			LOG_SYS_ERROR("Reloading asset that isn't a shader", aFileChanged);
			return;
		}

	}

	void ShaderCompiler::ForceRecompile()
	{
		myForceRecompile = true;
	}

	void ShaderCompiler::DontForceRecompile()
	{
		myForceRecompile = false;
	}

	std::vector<char> ShaderCompiler::LoadOrCompileFromFile(const std::string& aBaseFolder, const std::string& aFilePath, const std::string& aEntryPoint, const std::string& aCompiler, ShaderFlags aFlags)
	{
		static auto versionNumber = SHADER_VERSION;

		std::string binaryPath = myBakedFolderPath + aCompiler + "/" + aFilePath.substr(0, aFilePath.size() - std::filesystem::path(aFilePath).extension().string().size()) + ShaderTypes::PostfixFromFlags(aFlags) + ".cso";;
		std::string filePath = aBaseFolder + aFilePath;

		if (!myForceRecompile && std::filesystem::exists(binaryPath))
		{
			time_t binAge = tools::FileLastModified(binaryPath);
			time_t rawAge = tools::FileLastModified(filePath);

			if (binAge != -1 && rawAge != -1 && binAge > rawAge)
			{
				try
				{
					std::vector<char> binary = tools::ReadWholeFileBinary(binaryPath);
					if (binary.size() >= sizeof(versionNumber))
					{
						auto cmpResult = memcmp(binary.data(), &versionNumber, sizeof(versionNumber));
						if (cmpResult == 0)
						{
							std::vector<char> blob;
							size_t size = binary.size() - sizeof(versionNumber);
							blob.resize(size);
							memcpy(blob.data(), binary.data() + sizeof(versionNumber), size);
							return blob;
						}
					}
				}
				catch (const std::exception&)
				{
					LOG_SYS_ERROR("Couldn't open baked shader", binaryPath);
				}
			}
		}

		ID3DBlob* compiledShader = nullptr;
		ID3DBlob* errorBlob = nullptr;


		const UINT flags =
	#ifdef _DEBUG
			D3DCOMPILE_DEBUG |
	#endif // _DEBUG
			D3DCOMPILE_ENABLE_STRICTNESS |
			D3DCOMPILE_OPTIMIZATION_LEVEL3 |
			D3DCOMPILE_WARNINGS_ARE_ERRORS;

		ShaderDefines defines(aFlags);

		HRESULT result = D3DCompileFromFile(
			(std::wstring(filePath.begin(), filePath.end())).c_str(),
			defines.Get(),
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			aEntryPoint.c_str(),
			aCompiler.c_str(),
			flags,
			0,
			&compiledShader,
			&errorBlob);

		std::vector<char> blob;


		if (FAILED(result) || errorBlob)
		{
			LOG_SYS_ERROR("Could not compile shader", aEntryPoint, ShaderTypes::PostfixFromFlags(aFlags), aFilePath);
			if (errorBlob)
			{
				LOG_SYS_ERROR((char*)errorBlob->GetBufferPointer(), aEntryPoint, ShaderTypes::PostfixFromFlags(aFlags), aFilePath);
				errorBlob->Release();
			}
		}
		else
		{
			blob.resize(compiledShader->GetBufferSize());
			memcpy(blob.data(), compiledShader->GetBufferPointer(), compiledShader->GetBufferSize());

			std::filesystem::create_directories(binaryPath.substr(0, binaryPath.size() - std::filesystem::path(binaryPath).filename().string().size()));

			std::ofstream csoFile = std::ofstream(binaryPath, std::ios::binary | std::ios::out);

			csoFile.write(reinterpret_cast<const char*>(&versionNumber), sizeof(versionNumber));
			csoFile.write(blob.data(), blob.size());
		}

		return blob;
	}
}