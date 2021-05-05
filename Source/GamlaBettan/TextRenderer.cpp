#include "pch.h"
#include "TextRenderer.h"
#include "SpriteBatch.h"
#include "DirectX11Framework.h"
#include "TextInstance.h"

TextRenderer::TextRenderer() :
	mySpriteBatch(nullptr)
{
}

TextRenderer::~TextRenderer()
{
	SAFE_DELETE(mySpriteBatch);
}

bool TextRenderer::Init(DirectX11Framework* aFramework)
{
	mySpriteBatch = new DirectX::SpriteBatch(aFramework->GetContext());

	if (!mySpriteBatch)
	{
		SYSERROR("Could not create sprite batch in TextRenderer.Init().", "");
		return false;
	}

	return true;
}

void TextRenderer::Render(const std::vector<TextInstance*>& aTextList)
{
	mySpriteBatch->Begin();

	for (int i = aTextList.size() - 1; i >= 0; i--)
	{
		if (aTextList[i]->GetShouldDraw())
		{
			aTextList[i]->Render(mySpriteBatch);
		}
	}

	mySpriteBatch->End();
}
