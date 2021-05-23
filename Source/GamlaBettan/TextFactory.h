#pragma once
#include <string>
#include "Vector.hpp"
#include "Observer.hpp"

class TextInstance;
class DirectX11Framework;
class SpriteRenderer;
class SpriteFactory;
struct ID3D11Device;

class TextFactory
{
public:
	TextFactory();
	~TextFactory();
	bool Init(SpriteRenderer* aRenderer, SpriteFactory* aSpriteFactory);

	TextInstance* CreateText();
	TextInstance* CreateToolTip(class SpriteInstance* aBackground, const V2F& aBuffer, const std::string& someText = "", const std::string& aFontPath = "default.spritefont");
	TextInstance* CreateToolTip(const std::string& aBackgroundPath, const V2F& aBuffer, const std::string& someText, const std::string& aFontPath);


private:
	ID3D11Device* myDevicePtr;
	SpriteFactory* mySpriteFactoryPtr;
};

