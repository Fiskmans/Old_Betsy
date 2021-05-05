#pragma once
#include <string>
#include <unordered_map>
#include "Observer.hpp"
#include "Vector2.hpp"

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
	SpriteInstance* CreateVideoSprite();


private:
	Sprite* GetSprite(const std::string& aDDSPath, const bool aIsMovie);
	Sprite* LoadSprite(const std::string& aDDSPath, const bool aIsMovie);

	ID3D11Device* myDevice;
	std::unordered_map<std::string, Sprite*> mySprites;
	const std::string myTriangleName = "EngineTriangle";
	const std::string myVideoSpritePath = "Data\\Textures\\video.dds";
};