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
	virtual V2f GetSize() const;

	virtual void SetText(const std::wstring& aText);
	virtual void SetText(const std::string& aText);
	virtual const std::wstring& GetWideText() const;
	virtual std::string GetSlimText() const;

	virtual void SetTitle(const std::wstring& aTitle);
	virtual void SetTitle(const std::string& aTitle);
	virtual const std::wstring& GetWideTitle() const;
	virtual std::string GetSlimTitle() const;

	const V2f& GetPixelPosition() const;
	void SetPixelPosition(const V2f& aPosition);

	V2f GetPosition() const;
	void SetPosition(const V2f& aPosition);

	const V4F& GetColor() const;
	void SetColor(const V4F& aColor);

	const V4F& GetTitleColor() const;
	void SetTitleColor(const V4F& aColor);

	const V2f& GetScale() const;
	void SetScale(const V2f& aScale);

	const V2f& GetTitleScale() const;
	void SetTitleScale(const V2f& aScale);

	float GetRotation() const;
	void SetRotation(const float aRotation);

	const V2f& GetPivot() const;
	void SetPivot(const V2f& aPivot);

	const V2f& GetBuffer() const;
	void SetBuffer(const V2f& aBuffer);

	const TextEffect GetEffect() const;
	void SetEffect(const TextEffect anEffect);

	const SpriteInstance* GetBackground() const;
	void SetBackground(SpriteInstance* aBackground);

	bool GetShouldDraw() const;
	void SetShouldDraw(const bool aFlag);

	virtual void Render(DirectX::SpriteBatch* aSpriteBatch);

	const bool HadBeenAddedToScene() const;
	void SetAddedToSceneStatus(const bool aFlag);

	const V2f GetBackgroundSize();

protected:
	friend class TextFactory;

	V4F myColor;
	V4F myTitleColor;
	V2f myPosition;
	V2f myScale;
	V2f myTitleScale;
	V2f myPivot;
	V2f myBackgroundBuffer;

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
	virtual V2f GetTitleSize() const;
	V2f GetSize(const std::wstring& someText, const std::wstring& aTitle) const;
};



//     V         V
//      \       /
//       (ÅEw ÅE
//      ///   \\\