#pragma once

class FontData;

namespace DirectX
{
	class SpriteFont;
	class SpriteBatch;
}

class SpriteInstance;
class SpriteRenderer;

enum class TextEffect : uint32_t
{
	None = 0,
	FlipHorizontally = 1,
	FlipVertically = 2,
	FlipBoth = FlipVertically | FlipHorizontally
};

class TextInstance
{
public:
	TextInstance();
	TextInstance(const AssetHandle& aFont);
	~TextInstance();

	float GetWidth() const;
	float GetHeight() const;
	virtual V2F GetSize() const;

	virtual void SetText(const std::wstring& aText);
	virtual void SetText(const std::string& aText);
	virtual const std::wstring& GetWideText() const;
	virtual std::string GetSlimText() const;

	virtual void SetTitle(const std::wstring& aTitle);
	virtual void SetTitle(const std::string& aTitle);
	virtual const std::wstring& GetWideTitle() const;
	virtual std::string GetSlimTitle() const;

	const V2F& GetPixelPosition() const;
	void SetPixelPosition(const V2F& aPosition);

	V2F GetPosition() const;
	void SetPosition(const V2F& aPosition);

	const V4F& GetColor() const;
	void SetColor(const V4F& aColor);

	const V4F& GetTitleColor() const;
	void SetTitleColor(const V4F& aColor);

	const V2F& GetScale() const;
	void SetScale(const V2F& aScale);

	const V2F& GetTitleScale() const;
	void SetTitleScale(const V2F& aScale);

	float GetRotation() const;
	void SetRotation(const float aRotation);

	const V2F& GetPivot() const;
	void SetPivot(const V2F& aPivot);

	const V2F& GetBuffer() const;
	void SetBuffer(const V2F& aBuffer);

	const TextEffect GetEffect() const;
	void SetEffect(const TextEffect anEffect);

	const SpriteInstance* GetBackground() const;
	void SetBackground(SpriteInstance* aBackground);

	bool GetShouldDraw() const;
	void SetShouldDraw(const bool aFlag);

	virtual void Render(DirectX::SpriteBatch* aSpriteBatch);

	const bool HadBeenAddedToScene() const;
	void SetAddedToSceneStatus(const bool aFlag);

	const V2F GetBackgroundSize();

protected:
	friend class TextFactory;

	V4F myColor;
	V4F myTitleColor;
	V2F myPosition;
	V2F myScale;
	V2F myTitleScale;
	V2F myPivot;
	V2F myBackgroundBuffer;

	std::wstring myText;
	std::wstring myTitle;

	float myRotation;

	bool myShouldDraw;
	bool myIsAddedToScene;

	TextEffect myEffect;

	AssetHandle mySpriteFont;

	SpriteInstance* myBackground;
	static SpriteRenderer* ourBackgroundRendererPtr;

	void CalculateBackgroundSize();
	virtual V2F GetTitleSize() const;
	V2F GetSize(const std::wstring& someText, const std::wstring& aTitle) const;
};



//     V         V
//      \       /
//       (ÅEw ÅE
//      ///   \\\