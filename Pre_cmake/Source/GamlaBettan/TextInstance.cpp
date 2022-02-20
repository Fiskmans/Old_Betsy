#include "pch.h"
#include "TextInstance.h"
#include "SpriteRenderer.h"
#include "SpriteInstance.h"
#include "SpriteFontInclude.h"
#include "TimeHelper.h"
#include "Sprite.h"
#include "WindowHandler.h"

SpriteRenderer* TextInstance::ourBackgroundRendererPtr;

TextInstance::TextInstance() :
	myTitle(),
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
	myText(),
	myTitle(),
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

			myBackground->SetPosition((myPosition + (myBackgroundBuffer * myPivot)) / static_cast<V2f>(WindowHandler::GetInstance().GetSize()));
			myBackground->SetRotation(myRotation);

			ourBackgroundRendererPtr->Render(sprite);

			sprite.clear();
		}


		mySpriteFont.GetAsFont()->DrawString(aSpriteBatch, myText.c_str(), ToShitVector(myPosition - (myBackgroundBuffer * myPivot) + (myBackgroundBuffer * 0.5f) + V2f(0.f, GetTitleSize().y)), ToShitVector(myColor), myRotation, ToShitVector(GetSize() * myPivot), ToShitVector(myScale), CAST(DirectX::SpriteEffects, myEffect));

		if (!myTitle.empty())
		{
			mySpriteFont.GetAsFont()->DrawString(aSpriteBatch, myTitle.c_str(), ToShitVector(myPosition - (myBackgroundBuffer * myPivot) + (myBackgroundBuffer * 0.5f) + GetTitleSize() * (myPivot * V2f(myTitleScale.x - myScale.x, 0.5f))), ToShitVector(myTitleColor), myRotation, ToShitVector(GetSize() * myPivot), ToShitVector(myTitleScale), CAST(DirectX::SpriteEffects, myEffect));

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

const V2f TextInstance::GetBackgroundSize()
{
	return myBackground->GetSizeOnScreen();
}

void TextInstance::CalculateBackgroundSize()
{
	myBackground->SetScale((GetSize() + myBackgroundBuffer * 2.f) / myBackground->GetImageSize());
}

V2f TextInstance::GetTitleSize() const
{
	return GetSize(L"", myTitle);
}

V2f TextInstance::GetSize() const
{
	return GetSize(myText, myTitle);
}

V2f TextInstance::GetSize(const std::wstring& aText, const std::wstring& aTitle) const
{
	V2f size = FromShitVector(mySpriteFont.GetAsFont()->MeasureString(aText.c_str())) * myScale;
	V2f titleSize = (aTitle.empty() ? V2f() : FromShitVector(mySpriteFont.GetAsFont()->MeasureString(aTitle.c_str())) * myTitleScale);

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
	std::string out;
	out.reserve(myText.size());
	for (const wchar_t& c : myText)
	{
		out += static_cast<char>(c);
	}

	return out;
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
	std::string out;
	out.reserve(myTitle.size());
	for (const wchar_t& c : myTitle)
	{
		out += static_cast<char>(c);
	}

	return out;
}

const V2f& TextInstance::GetPixelPosition() const
{
	return myPosition;
}

void TextInstance::SetPixelPosition(const V2f& aPosition)
{
	myPosition = aPosition;
}

V2f TextInstance::GetPosition() const
{
	return myPosition / static_cast<V2f>(WindowHandler::GetInstance().GetSize());
}

void TextInstance::SetPosition(const V2f& aPosition)
{
	SetPixelPosition(aPosition * static_cast<V2f>(WindowHandler::GetInstance().GetSize()));
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

const V2f& TextInstance::GetScale() const
{
	return myScale;
}

const V2f& TextInstance::GetTitleScale() const
{
	return myTitleScale;
}

void TextInstance::SetTitleScale(const V2f& aScale)
{
	myTitleScale = aScale;
}

void TextInstance::SetScale(const V2f& aScale)
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

const V2f& TextInstance::GetPivot() const
{
	return myPivot;
}

void TextInstance::SetPivot(const V2f& aPivot)
{
	myPivot = aPivot;

	if (myBackground)
	{
		myBackground->SetPivot(myPivot);
	}
}

const V2f& TextInstance::GetBuffer() const
{
	return myBackgroundBuffer;
}

void TextInstance::SetBuffer(const V2f& aBuffer)
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