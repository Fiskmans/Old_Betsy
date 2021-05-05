#pragma once
#include <string>

struct ID3D11Device;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

class PixelShader
{
	friend void ReloadPixelShader(ID3D11Device* aDevice, const std::string& aPath, size_t aFlags);
public:
	PixelShader(ID3D11PixelShader* aPixelShader);
	void ReleaseShader();
	operator ID3D11PixelShader* ();

#ifndef _RETAIL
	std::string myDebugShaderPath = "";
#endif

private:
	ID3D11PixelShader* myShader;
};

class VertexShader
{
	friend void ReloadVertexShader(ID3D11Device* aDevice, const std::string& aPath, size_t aFlags);
public:
	VertexShader(ID3D11VertexShader* aVertexShader);
	void ReleaseShader();
	operator ID3D11VertexShader* ();

#ifndef _RETAIL
	std::string myDebugShaderPath = "";
#endif

private:
	ID3D11VertexShader* myShader;
};

