#pragma once
#include <vector>

class DirectX11Framework;
class TextInstance;

namespace DirectX
{
	class SpriteBatch;
}

class TextRenderer
{
public:
	TextRenderer();
	~TextRenderer();

	bool Init(DirectX11Framework* aFramework);

	void Render(const std::vector<TextInstance*>& aTextList);

private:
	DirectX::SpriteBatch* mySpriteBatch;
};

