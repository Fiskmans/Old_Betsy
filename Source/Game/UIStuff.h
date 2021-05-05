#pragma once

class Scene;
class SpriteFactory;
class SpriteInstance;
class TextFactory;
class TextInstance;
class AbilityData;
class Camera;
class GemStone;
struct Ability;

class DamageNumber
{
public:
	DamageNumber(V2F aPosition, float aLifeTime, int someDamage, bool aIsCrit, TextFactory* aTextFactory, Scene* aScene);
	~DamageNumber();

	void Update(float aDeltaTime);

private:
	friend class UIManager;

	float myLifeTime;
	Scene* myScene;
	TextInstance* myText;
	V2F mySpeed;
};

class AuraHolder
{
public:

	AuraHolder(Scene* aScene);
	~AuraHolder();

	void Update(float aDeltaTime, V2F aPos);
	void AddSpritesToScene(Scene* aScene);

	void AddSprite(AuraType aAuraType, SpriteInstance* aSprite);
	void RemoveSprite(AuraType aAuraType);

private:
	V2F myPosition;
	std::unordered_map<int, SpriteInstance*> myAuraSprites;
	Scene* myScene;
};