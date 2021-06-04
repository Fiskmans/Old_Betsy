#pragma once
#include "Observer.hpp"

class SpriteInstance;
class Sprite;
struct ID3D11Device;
class DirectX11Framework;

class SpriteFactory 
{
public:

	SpriteFactory();
	~SpriteFactory();
	bool Init(DirectX11Framework* aFramework);

	SpriteInstance* CreateSprite(const std::string& aDDSPath);

private:
	Sprite* GetSprite(const std::string& aDDSPath);
	Sprite* LoadSprite(const std::string& aDDSPath);

	ID3D11Device* myDevice;
	std::unordered_map<std::string, Sprite*> mySprites;
	const std::string myTriangleName = "EngineTriangle";
};