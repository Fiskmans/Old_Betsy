#include "pch.h"
#include "ShaderCompiler.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include "Macros.h"
#include <fstream>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <time.h>
#include "ShaderTypes.h"
#include <unordered_map>
#include <DirectX11Framework.h>

#if USEFILEWATHCER
#include <FileWatcher.h>
#endif

size_t FileAge(const std::string& aFilepath)
{
	struct _stat64i32 s;
	if (_stat64i32(aFilepath.c_str(),&s) == 0)
	{
		time_t t = _time64(&t); // Now();
		return t - s.st_mtime;
	}
	else
	{
		return -1;
	}
}

std::unordered_map<std::string, PixelShader*>& LoadedPixelShaders()
{
	static std::unordered_map<std::string, PixelShader*> shaders;
	return shaders;
}

std::unordered_map<std::string, VertexShader*>& LoadedVertexShaders()
{
	static std::unordered_map<std::string, VertexShader*> shaders;
	return shaders;
}

std::unordered_map<std::string, std::vector<char>>& LoadedBlobs()
{
	static std::unordered_map<std::string, std::vector<char>> blobs;
	return blobs;
}
#if USEFILEWATHCER
Tools::FileWatcher ShaderFileWatcher;
std::unordered_map<std::string, bool> globalIsFileWatched;
#endif

void ReleaseAllShaders()
{
	for (auto& i : LoadedPixelShaders())
	{
		i.second->ReleaseShader();
		SAFE_DELETE(i.second);
	} 
	LoadedPixelShaders().clear();
	for (auto& i : LoadedVertexShaders())
	{
		i.second->ReleaseShader();
		SAFE_DELETE(i.second);
	}
	LoadedVertexShaders().clear();
	LoadedBlobs().clear();
}

PixelShader* GetPixelShader(ID3D11Device* aDevice, const std::string& aPath, size_t aFlags)
{
	std::experimental::filesystem::path BasePath(aPath);
	std::string binaryPath = aPath.substr(0, aPath.size() - BasePath.extension().string().size()) + ShaderTypes::PostfixFromFlags(aFlags) + "_ps.cso";
	if (LoadedPixelShaders().count(binaryPath) == 0)
	{
		std::fstream csoFile;
		HRESULT result;
		std::vector<char> blob;

		csoFile.open(binaryPath, std::ios::binary | std::ios::in | std::ios::ate);

		if (csoFile && FileAge(binaryPath) > FileAge(aPath) && false)
		{
			SYSINFO("Loading pixelshader: " + aPath + " [" + ShaderTypes::PostfixFromFlags(aFlags) + "] From File");
			std::streamsize binarySize = csoFile.tellg();
			csoFile.seekg(0, std::ios::beg);
			blob.resize(binarySize);
			csoFile.read(blob.data(), binarySize);
		}
		else
		{
			UINT flags = 0;
			flags |= D3DCOMPILE_ENABLE_STRICTNESS;
			flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
			flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#ifdef _DEBUG
			flags |= D3DCOMPILE_DEBUG;
#endif // _DEBUG

			ID3DBlob* compiledShader = nullptr;
			ID3DBlob* errorBlob = nullptr;

			{
				std::ifstream s(aPath);
				if (!s)
				{
					SYSERROR("file could not be opened",aPath);
					return nullptr;
				}
			}

			SYSINFO("(Re)compiling pixelshader: " + aPath + " [" + ShaderTypes::PostfixFromFlags(aFlags) + "]");
			result = D3DCompileFromFile((std::wstring(aPath.begin(), aPath.end())).c_str(), ShaderTypes::DefinesFromFlags(aFlags), D3D_COMPILE_STANDARD_FILE_INCLUDE, "pixelShader", "ps_5_0", flags, 0, &compiledShader, &errorBlob);
			if (SUCCEEDED(result))
			{
				blob.resize(compiledShader->GetBufferSize());
				memcpy(blob.data(), compiledShader->GetBufferPointer(), compiledShader->GetBufferSize());

				csoFile.open(binaryPath, std::ios::binary | std::ios::out);
				csoFile.write(blob.data(), blob.size());
			}
			if (compiledShader)
			{
				compiledShader->Release();
			}
			if (errorBlob)
			{
				SYSERROR("Could not compile pixel shader with flags: [" + ShaderTypes::PostfixFromFlags(aFlags) + "]",aPath);
				SYSERROR((char*)errorBlob->GetBufferPointer(),aPath);
				errorBlob->Release();
			}
		}
		ID3D11PixelShader* shader;
		DirectX11Framework::AddMemoryUsage(blob.size(), std::filesystem::path(aPath).filename().string(), "Pixel Shader");
		result = aDevice->CreatePixelShader(blob.data(), blob.size(), nullptr, &shader);
		if (FAILED(result))
		{
			return nullptr;
		}
		LoadedPixelShaders()[binaryPath] = new PixelShader(shader);
#if USEFILEWATHCER
		if (!globalIsFileWatched[aPath])
		{
			ShaderFileWatcher.RegisterCallback(aPath, std::bind(ReloadPixelShader, aDevice, std::placeholders::_1, aFlags));
			globalIsFileWatched[aPath] = true;
		}
#endif // USEFILEWATHCER


#ifndef  _RETAIL
		LoadedPixelShaders()[binaryPath]->myDebugShaderPath = aPath;
#endif // ! _RETAIL

	}
	return LoadedPixelShaders()[binaryPath];
}

VertexShader* GetVertexShader(ID3D11Device* aDevice, const std::string& aPath, std::vector<char>& aVsBlob, size_t aFlags)
{
	std::experimental::filesystem::path BasePath(aPath);
	std::string binaryPath = aPath.substr(0, aPath.size() - BasePath.extension().string().size()) + ShaderTypes::PostfixFromFlags(aFlags) + "_vs.cso";

	if (LoadedVertexShaders().count(binaryPath) == 0)
	{
		std::fstream csoFile;
		HRESULT result;
		std::vector<char> blob;

		csoFile.open(binaryPath, std::ios::binary | std::ios::in | std::ios::ate);

		if (csoFile && FileAge(binaryPath) > FileAge(aPath) && false)
		{
			SYSINFO("Loading vertexshader: " + aPath + " [" + ShaderTypes::PostfixFromFlags(aFlags) + "] From File");
			std::streamsize binarySize = csoFile.tellg();
			csoFile.seekg(0, std::ios::beg);
			blob.resize(binarySize);
			csoFile.read(blob.data(), binarySize);
		}
		else
		{
			UINT flags = 0;
			flags |= D3DCOMPILE_ENABLE_STRICTNESS;
			flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
			flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#ifdef _DEBUG
			flags |= D3DCOMPILE_DEBUG;
#endif // _DEBUG

			ID3DBlob* compiledShader = nullptr;
			ID3DBlob* errorBlob = nullptr;

			{
				std::ifstream s(aPath);
				if (!s)
				{
					SYSERROR("file could not be opened",aPath);
					return nullptr;
				}
			}
			SYSINFO("(Re)compiling vertexshader: " + aPath + " [" + ShaderTypes::PostfixFromFlags(aFlags) + "]");
			result = D3DCompileFromFile((std::wstring(aPath.begin(), aPath.end())).c_str(),ShaderTypes::DefinesFromFlags(aFlags), D3D_COMPILE_STANDARD_FILE_INCLUDE, "vertexShader", "vs_5_0", flags, 0, &compiledShader, &errorBlob);

			if (SUCCEEDED(result))
			{
				blob.resize(compiledShader->GetBufferSize());
				memcpy(blob.data(), compiledShader->GetBufferPointer(), compiledShader->GetBufferSize());

				csoFile.open(binaryPath, std::ios::binary | std::ios::out);
				csoFile.write(blob.data(), blob.size());
			}
			if (errorBlob)
			{
				SYSERROR((char*)errorBlob->GetBufferPointer(), BasePath.string());
				SYSERROR("Could not compile pixel shader with flags: [" + ShaderTypes::PostfixFromFlags(aFlags) + "]", BasePath.string());
				errorBlob->Release();
			}
		}
		ID3D11VertexShader* shader;
		DirectX11Framework::AddMemoryUsage(blob.size(), std::filesystem::path(aPath).filename().string(), "Vertex Shader");
		result = aDevice->CreateVertexShader(blob.data(), blob.size(), nullptr, &shader);
		if (FAILED(result))
		{
			const _D3D_SHADER_MACRO* macros = ShaderTypes::DefinesFromFlags(aFlags);
			SYSERROR("could oh noes ICANHAZ stdio?","");
			return nullptr;
		}
		LoadedBlobs()[binaryPath] = blob;
		LoadedVertexShaders()[binaryPath] = new VertexShader(shader);
#if USEFILEWATHCER
		if (!globalIsFileWatched[aPath])
		{
			ShaderFileWatcher.RegisterCallback(aPath, std::bind(ReloadVertexShader, aDevice, std::placeholders::_1, aFlags));
			globalIsFileWatched[aPath] = true;
		}
#endif // USEFILEWATHCER
#ifndef  _RETAIL
		LoadedVertexShaders()[binaryPath]->myDebugShaderPath = aPath;
#endif // ! _RETAIL
	}


	aVsBlob = LoadedBlobs()[binaryPath];
	return LoadedVertexShaders()[binaryPath];
}

void ReloadPixelShader(ID3D11Device* aDevice, const std::string& aPath,size_t aFlags)
{
	std::string binaryFile = aPath.substr(0, aPath.length() - std::experimental::filesystem::path(aPath).extension().string().length()) + ShaderTypes::PostfixFromFlags(aFlags) + "_ps.cso";

	if (LoadedPixelShaders().find(binaryFile) != LoadedPixelShaders().end())
	{
		PixelShader* start = LoadedPixelShaders()[binaryFile];
		LoadedPixelShaders().erase(binaryFile);
		PixelShader* end = GetPixelShader(aDevice, aPath, aFlags);
		if (end)
		{
			start->ReleaseShader();
			start->myShader = end->myShader;
			LoadedPixelShaders()[binaryFile] = start;
			delete end;
		}
		else
		{
			SYSWARNING("Failed to reload",aPath);
			LoadedPixelShaders()[binaryFile] = start;
		}
	}
	else
	{
		SYSWARNING("trying to reload shader thats not loaded",aPath);
	}
}

void ReloadVertexShader(ID3D11Device* aDevice, const std::string& aPath, size_t aFlags)
{
	std::string binaryFile = aPath.substr(0, aPath.length() - std::experimental::filesystem::path(aPath).extension().string().length()) + ShaderTypes::PostfixFromFlags(aFlags) + "_vs.cso";

	if (LoadedVertexShaders().find(binaryFile) != LoadedVertexShaders().end())
	{
		VertexShader* start = LoadedVertexShaders()[binaryFile];
		LoadedVertexShaders().erase(binaryFile);
		std::vector<char> _;
		VertexShader* end = GetVertexShader(aDevice, aPath, _,aFlags);
		if (end)
		{
			start->ReleaseShader();
			start->myShader = end->myShader;
			LoadedVertexShaders()[binaryFile] = start;
			delete end;
		}
		else
		{
			SYSWARNING("Failed to reload",aPath);
			LoadedVertexShaders()[binaryFile] = start;
		}
	}
	else
	{
		SYSWARNING("trying to reload shader thats not loaded", binaryFile);
	}
}


bool LoadPixelShader(ID3D11Device* aDevice, std::string aFilePath, ID3D11PixelShader*& aShaderOutput)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;


	HRESULT hr = D3DCompileFromFile((std::wstring(aFilePath.begin(), aFilePath.end())).c_str(), ShaderTypes::DefinesFromFlags(0), D3D_COMPILE_STANDARD_FILE_INCLUDE, "pixelShader", "ps_5_0", flags, 0, &shaderBlob, &errorBlob);

	if (FAILED(hr))
	{
		SYSERROR("Failed to compile pixel shader", aFilePath);
		if (errorBlob)
		{
			LOGERROR((char*)errorBlob->GetBufferPointer(),"");
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return false;
	}

	hr = aDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &aShaderOutput);

	shaderBlob->Release();

	if (FAILED(hr))
	{
		SYSERROR("Could not create pixelshader on the graphics card","");
		return false;
	}

	return true;
}

bool LoadGeometryShader(ID3D11Device* aDevice, std::string aFilePath, ID3D11GeometryShader*& aShaderOutput)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;


	HRESULT hr = D3DCompileFromFile((std::wstring(aFilePath.begin(), aFilePath.end())).c_str(), ShaderTypes::DefinesFromFlags(0), D3D_COMPILE_STANDARD_FILE_INCLUDE, "geometryShader", "gs_5_0", flags, 0, &shaderBlob, &errorBlob);

	if (FAILED(hr))
	{
		SYSERROR("Failed to compile pixel shader","");
		if (errorBlob)
		{
			LOGERROR((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return false;
	}

	hr = aDevice->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &aShaderOutput);

	shaderBlob->Release();

	if (FAILED(hr))
	{
		SYSERROR("Could not create pixelshader on the graphics card","");
		return false;
	}

	return true;
}

bool LoadVertexShader(ID3D11Device* aDevice, std::string aFilePath, ID3D11VertexShader*& aShaderOutput, void* aCompiledOutput, size_t aFlags)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG 
	flags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob** shaderBlob = static_cast<ID3DBlob**>(aCompiledOutput);
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompileFromFile((std::wstring(aFilePath.begin(), aFilePath.end())).c_str(), ShaderTypes::DefinesFromFlags(aFlags), D3D_COMPILE_STANDARD_FILE_INCLUDE, "vertexShader", "vs_5_0", flags, 0, shaderBlob, &errorBlob);

	if (FAILED(hr))
	{
		SYSERROR("Failed to compile Vertex shader","");
		if (errorBlob)
		{
			LOGERROR((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if ((*shaderBlob))
		{
			(*shaderBlob)->Release();
		}

		return false;
	}

	hr = aDevice->CreateVertexShader((*shaderBlob)->GetBufferPointer(), (*shaderBlob)->GetBufferSize(), nullptr, &aShaderOutput);


	if (FAILED(hr))
	{
		SYSERROR("Could not create vertexshader on the graphics card",aFilePath);
		return false;
	}

	return true;
}

bool CompilePixelShader(ID3D11Device* aDevice, std::string aData, ID3D11PixelShader*& aShaderOutput, size_t aFlags)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG 
	flags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompile(aData.c_str(), aData.length(), NULL, ShaderTypes::DefinesFromFlags(aFlags), D3D_COMPILE_STANDARD_FILE_INCLUDE, "pixelShader", "ps_5_0", flags, 0, &shaderBlob, &errorBlob);

	if (FAILED(hr))
	{
		SYSERROR("Failed to compile Pixel shader","");
		if (errorBlob)
		{
			LOGERROR((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return false;
	}

	hr = aDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &aShaderOutput);

	shaderBlob->Release();

	if (FAILED(hr))
	{
		SYSERROR("Could not create pixelShader on the graphics card","");
		return false;
	}

	return true;
}
bool CompileGeometryShader(ID3D11Device* aDevice, std::string aData, ID3D11GeometryShader*& aShaderOutput)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG 
	flags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompile(aData.c_str(), aData.length(), NULL, ShaderTypes::DefinesFromFlags(0), D3D_COMPILE_STANDARD_FILE_INCLUDE, "geometryShader", "gs_5_0", flags, 0, &shaderBlob, &errorBlob);

	if (FAILED(hr))
	{
		SYSERROR("Failed to compile Geometry shader","");
		if (errorBlob)
		{
			LOGERROR((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return false;
	}

	hr = aDevice->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &aShaderOutput);

	shaderBlob->Release();

	if (FAILED(hr))
	{
		SYSERROR("Could not create pixelShader on the graphics card","");
		return false;
	}

	return true;
}
bool CompileVertexShader(ID3D11Device* aDevice, std::string aData, ID3D11VertexShader*& aShaderOutput, void* aCompiledOutput, size_t aFlags)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG 
	flags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob** shaderBlob = static_cast<ID3DBlob**>(aCompiledOutput);
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompile(aData.c_str(), aData.length(), NULL, ShaderTypes::DefinesFromFlags(aFlags), D3D_COMPILE_STANDARD_FILE_INCLUDE, "vertexShader", "vs_5_0", flags, 0, shaderBlob, &errorBlob);

	if (FAILED(hr))
	{
		SYSERROR("Failed to compile Vertex shader","");
		if (errorBlob)
		{
			LOGERROR((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (*shaderBlob)
		{
			(*shaderBlob)->Release();
		}

		return false;
	}

	hr = aDevice->CreateVertexShader((*shaderBlob)->GetBufferPointer(), (*shaderBlob)->GetBufferSize(), nullptr, &aShaderOutput);

	if (FAILED(hr))
	{
		SYSERROR("Could not create vertexshader on the graphics card","");
		return false;
	}

	return true;
}

void FlushShaderChanges()
{
#if USEFILEWATHCER
	ShaderFileWatcher.FlushChanges();
#endif // USEFILEWATHCER
}
