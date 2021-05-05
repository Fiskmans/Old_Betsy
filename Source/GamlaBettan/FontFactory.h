#pragma once
#include <unordered_map>

namespace DirectX
{
	class SpriteFont;
}

class FontFactory
{
public:
	static void SetDevice(DirectX11Framework* aFramework);

protected:
	static DirectX::SpriteFont* GetFont(const std::string& aFontPath);
	static DirectX::SpriteFont* LoadFont(const std::string& aFontPath);

	static std::unordered_map<std::string, DirectX::SpriteFont*> ourFonts;
	static ID3D11Device* ourDevicePtr;

	FontFactory() = default;
	~FontFactory() = default;
private:
};