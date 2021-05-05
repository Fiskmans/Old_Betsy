#include "pch.h"
#include "Shaders.h"
#include <d3d11.h>

PixelShader::PixelShader(ID3D11PixelShader* aPixelShader)
{
	myShader = aPixelShader;
}

void PixelShader::ReleaseShader()
{
	SAFE_RELEASE(myShader);
}

PixelShader::operator ID3D11PixelShader* ()
{
	return myShader;
}

VertexShader::VertexShader(ID3D11VertexShader* aVertexShader)
{
	myShader = aVertexShader;
}

void VertexShader::ReleaseShader()
{
	SAFE_RELEASE(myShader);
}

VertexShader::operator ID3D11VertexShader* ()
{
	return myShader;
}
