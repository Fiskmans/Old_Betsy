#pragma once
#include <string>
#include "ShaderFlags.h"
#include <vector>
#include "Shaders.h"

struct ID3D11Device;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11GeometryShader;

void ReleaseAllShaders();

PixelShader* GetPixelShader(ID3D11Device* aDevice, const std::string& aPath, size_t aFlags = ShaderFlags::None);
VertexShader* GetVertexShader(ID3D11Device* aDevice, const std::string& aPath, std::vector<char>& aVsBlob, size_t aFlags = ShaderFlags::None);

void ReloadPixelShader(ID3D11Device* aDevice, const std::string& aPath, size_t aFlags);
void ReloadVertexShader(ID3D11Device* aDevice, const std::string& aPath, size_t aFlags);

bool LoadGeometryShader(ID3D11Device* aDevice, std::string aFilePath, ID3D11GeometryShader*& aShaderOutput);

bool CompilePixelShader(ID3D11Device* aDevice, std::string aData, ID3D11PixelShader*& aShaderOutput, size_t aFlags = ShaderFlags::None);
bool CompileGeometryShader(ID3D11Device* aDevice, std::string aData, ID3D11GeometryShader*& aShaderOutput);
bool CompileVertexShader(ID3D11Device* aDevice, std::string aData, ID3D11VertexShader*& aShaderOutput, void* aCompiledOutput, size_t aFlags = ShaderFlags::None);

void FlushShaderChanges();