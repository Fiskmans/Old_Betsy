#pragma once

struct ID3D11ShaderResourceView;
struct ID3D11Device;
enum D3D11_RESOURCE_MISC_FLAG;

class TextureLoader
{
public:
	TextureLoader(ID3D11Device* aDevice);

	Asset* LoadTexture(const std::string& aFile, bool aFailSilenty = false);
	Asset* LoadCubeTexture(const std::string& aFile);
	Asset* GeneratePerlin(V2ui aSize, V2f aScale = { 1.f, 1.f }, unsigned int aSeed = 0);

private:
	Asset* LoadTextureInternal(const std::string& aFile, D3D11_RESOURCE_MISC_FLAG aFlags, bool aFailSilenty);

	ID3D11Device* myDevice;
};
