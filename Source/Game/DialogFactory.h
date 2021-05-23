#pragma once
#include "Vector2.hpp"
#include "DialogInstance.h"

class TextInstance;
class DirectX11Framework;
class SpriteRenderer;
class SpriteFactory;
struct ID3D11Device;

class DialogFactory : public Observer
{
public:
	DialogFactory();
	~DialogFactory();

	bool Init(SpriteRenderer* aRenderer, SpriteFactory* aSpriteFactory);

	DialogInstance* CreateGameDialog(const std::string& aFilePath, const V2F& aBuffer, const std::string& aBackgroundPath = "Data/UI/DialogBackground.dds", const std::string& aFontPath = "Data/Fonts/dialogDefault.spritefont");

	void RecieveMessage(const Message& aMessage) override;

private:
	std::unordered_map<std::string, DialogInstance::DialogData> myLoadedDialogs;
	SpriteFactory* mySpriteFactoryPtr;

	bool LoadDialog(const std::string& aFilePath);
	const DialogInstance::DialogData* GetDialog(const std::string& aFilePath);

	int myCurrentDay;
};