#include "pch.h"
#include "TextInstance.h"
#include "SpriteRenderer.h"
#include "SpriteInstance.h"
#include "SpriteFontInclude.h"
#include "TimeHelper.h"
#include "Sprite.h"

SpriteRenderer* TextInstance::ourBackgroundRendererPtr;

TextInstance::TextInstance() :
	myTitle(L""),
	myColor({ 1.f, 1.f, 1.f, 1.f }),
	myTitleColor({ 1.f, 1.f, 1.f, 1.f }),
	myPosition({ 0.f, 0.f }),
	myScale({ 0.f, 0.f }),
	myTitleScale({ 0.f, 0.f }),
	myPivot({ 0.f, 0.f }),
	myBackgroundBuffer({ 0.f, 0.f }),
	myRotation(0.f),
	myShouldDraw(true),
	myIsAddedToScene(false),
	myEffect(TextEffect::None),
	myBackground(nullptr)
{
}

TextInstance::TextInstance(const AssetHandle& aFont) :
	mySpriteFont(aFont),
	myText(L""),
	myTitle(L""),
	myColor({ 1.f, 1.f, 1.f, 1.f }),
	myTitleColor({ 1.f, 1.f, 1.f, 1.f }),
	myPosition({ 0.f, 0.f }),
	myScale({ 1.f, 1.f }),
	myTitleScale({ 1.2f, 1.2f }),
	myPivot({ 0.f, 0.f }),
	myBackgroundBuffer({ 0.f, 0.f }),
	myRotation(0.f),
	myShouldDraw(true),
	myIsAddedToScene(false),
	myEffect(TextEffect::None),
	myBackground(nullptr)
{
}

TextInstance::~TextInstance()
{
	SAFE_DELETE(myBackground);
	myText.clear();
	myTitle.clear();
	WIPE(*this);
}

void TextInstance::Render(DirectX::SpriteBatch* aSpriteBatch)
{
	if (myShouldDraw && (!myText.empty() || !myTitle.empty()))
	{
		if (myBackground)
		{
			static std::vector<SpriteInstance*> sprite; //It's intentional trust me (SpriteRenderer.Render() only accepts vectors as of writing)
			sprite.push_back(myBackground);

			CalculateBackgroundSize();

			myBackground->SetPosition((myPosition + (myBackgroundBuffer * myPivot)) / Sprite::ourWindowSize);
			myBackground->SetRotation(myRotation);

			ourBackgroundRendererPtr->Render(sprite);

			sprite.clear();
		}


		mySpriteFont.GetAsFont()->DrawString(aSpriteBatch, myText.c_str(), ToShitVector(myPosition - (myBackgroundBuffer * myPivot) + (myBackgroundBuffer * 0.5f) + V2F(0.f, GetTitleSize().y)), ToShitVector(myColor), myRotation, ToShitVector(GetSize() * myPivot), ToShitVector(myScale), CAST(DirectX::SpriteEffects, myEffect));

		if (!myTitle.empty())
		{
			mySpriteFont.GetAsFont()->DrawString(aSpriteBatch, myTitle.c_str(), ToShitVector(myPosition - (myBackgroundBuffer * myPivot) + (myBackgroundBuffer * 0.5f) + GetTitleSize() * (myPivot * V2F(myTitleScale.x - myScale.x, 0.5f))), ToShitVector(myTitleColor), myRotation, ToShitVector(GetSize() * myPivot), ToShitVector(myTitleScale), CAST(DirectX::SpriteEffects, myEffect));

		}
	}
}

const bool TextInstance::HadBeenAddedToScene() const
{
	return myIsAddedToScene;
}

void TextInstance::SetAddedToSceneStatus(const bool aFlag)
{
	myIsAddedToScene = aFlag;
}

const V2F TextInstance::GetBackgroundSize()
{
	return myBackground->GetSizeOnScreen();
}

void TextInstance::CalculateBackgroundSize()
{
	myBackground->SetScale((GetSize() + myBackgroundBuffer * 2.f) / myBackground->GetImageSize());
}

V2F TextInstance::GetTitleSize() const
{
	return GetSize(L"", myTitle);
}

V2F TextInstance::GetSize() const
{
	return GetSize(myText, myTitle);
}

V2F TextInstance::GetSize(const std::wstring& someText, const std::wstring& aTitle) const
{
	V2F size = FromShitVector(mySpriteFont.GetAsFont()->MeasureString(someText.c_str())) * myScale;
	V2F titleSize = (aTitle.empty() ? V2F() : FromShitVector(mySpriteFont.GetAsFont()->MeasureString(aTitle.c_str())) * myTitleScale);

	size.y += titleSize.y;
	size.x = MAX(titleSize.x, size.x);

	return size;
}

float TextInstance::GetWidth() const
{
	return GetSize().x;
}

float TextInstance::GetHeight() const
{
	return GetSize().y;
}

void TextInstance::SetText(const std::wstring& aText)
{
	myText = aText;
}

void TextInstance::SetText(const std::string& aText)
{
	myText = std::wstring(aText.begin(), aText.end());
}

const std::wstring& TextInstance::GetWideText() const
{
	return myText;
}

std::string TextInstance::GetSlimText() const
{
	return std::string(myText.begin(), myText.end());
}

void TextInstance::SetTitle(const std::wstring& aTitle)
{
	myTitle = aTitle;
}

void TextInstance::SetTitle(const std::string& aTitle)
{
	myTitle = std::wstring(aTitle.begin(), aTitle.end());
}

const std::wstring& TextInstance::GetWideTitle() const
{
	return myTitle;
}

std::string TextInstance::GetSlimTitle() const
{
	return std::string(myTitle.begin(), myTitle.end());
}

const V2F& TextInstance::GetPixelPosition() const
{
	return myPosition;
}

void TextInstance::SetPixelPosition(const V2F& aPosition)
{
	myPosition = aPosition;
}

V2F TextInstance::GetPosition() const
{
	return V2F(myPosition.x / Sprite::ourWindowSize.x, myPosition.y / Sprite::ourWindowSize.y);
}

void TextInstance::SetPosition(const V2F& aPosition)
{
	SetPixelPosition({ aPosition.x * Sprite::ourWindowSize.x, aPosition.y * Sprite::ourWindowSize.y });
}

const V4F& TextInstance::GetColor() const
{
	return myColor;
}

void TextInstance::SetColor(const V4F& aColor)
{
	myColor = aColor;
}

const V4F& TextInstance::GetTitleColor() const
{
	return myTitleColor;
}

void TextInstance::SetTitleColor(const V4F& aColor)
{
	myTitleColor = aColor;
}

const V2F& TextInstance::GetScale() const
{
	return myScale;
}

const V2F& TextInstance::GetTitleScale() const
{
	return myTitleScale;
}

void TextInstance::SetTitleScale(const V2F& aScale)
{
	myTitleScale = aScale;
}

void TextInstance::SetScale(const V2F& aScale)
{
	myScale = aScale;
}

float TextInstance::GetRotation() const
{
	return myRotation;
}

void TextInstance::SetRotation(const float aRotation)
{
	myRotation = aRotation;
}

const V2F& TextInstance::GetPivot() const
{
	return myPivot;
}

void TextInstance::SetPivot(const V2F& aPivot)
{
	myPivot = aPivot;

	if (myBackground)
	{
		myBackground->SetPivot(myPivot);
	}
}

const V2F& TextInstance::GetBuffer() const
{
	return myBackgroundBuffer;
}

void TextInstance::SetBuffer(const V2F& aBuffer)
{
	myBackgroundBuffer = aBuffer;
}

const TextEffect TextInstance::GetEffect() const
{
	return myEffect;
}

void TextInstance::SetEffect(const TextEffect anEffect)
{
	myEffect = anEffect;
}

const SpriteInstance* TextInstance::GetBackground() const
{
	return myBackground;
}

void TextInstance::SetBackground(SpriteInstance* aBackground)
{
	if (myBackground)
	{
		delete myBackground;
	}

	myBackground = aBackground;
	myBackground->SetPivot(myPivot);
}

bool TextInstance::GetShouldDraw() const
{
	return myShouldDraw;
}

void TextInstance::SetShouldDraw(const bool aFlag)
{
	myShouldDraw = aFlag;
}