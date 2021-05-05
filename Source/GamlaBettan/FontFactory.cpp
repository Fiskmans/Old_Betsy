#include "pch.h"
#include "FontFactory.h"
#include "DirectX11Framework.h"
#include <SpriteFont.h>

std::unordered_map<std::string, DirectX::SpriteFont*>  FontFactory::ourFonts;
ID3D11Device* FontFactory::ourDevicePtr = nullptr;

void FontFactory::SetDevice(DirectX11Framework* aFramework)
{
	ourDevicePtr = aFramework->GetDevice();
}

DirectX::SpriteFont* FontFactory::GetFont(const std::string& aFontPath)
{
	if (ourFonts.find(aFontPath) == ourFonts.end())
	{
		if (!LoadFont(aFontPath))
		{
			SYSERROR("Failed to load in FontFactory.GetFont() ", aFontPath);
			return nullptr;
		}
	}

	return ourFonts[aFontPath];
}

DirectX::SpriteFont* FontFactory::LoadFont(const std::string& aFontPath)
{
	DirectX::SpriteFont* newFont = new DirectX::SpriteFont(ourDevicePtr, (std::wstring(aFontPath.begin(), aFontPath.end())).c_str());

	ourFonts[aFontPath] = newFont;
	return ourFonts[aFontPath];
}
