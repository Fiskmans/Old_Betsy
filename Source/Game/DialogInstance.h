#pragma once
#include "TextInstance.h"

class DialogInstance : public TextInstance, public Observer
{
public:
	struct DialogData
	{
		struct Snippet
		{
			std::wstring speaker;
			std::wstring text;
		};

		std::vector<std::vector<Snippet>> data;
		bool isDaily = false;
		int lastTriggered = 0;
		int currentIndex = 0;

#ifndef _RETAIL
		std::string filePath;
#endif // !_RETAIL
	};

	DialogInstance();
	~DialogInstance();

	void Init(DirectX::SpriteFont* aFont, const DialogData* someDialog, const V2F& aBuffer, SpriteInstance* aBackground);

	void Render(DirectX::SpriteBatch* aSpriteBatch) override;

	void RecieveMessage(const Message& aMessage) override;

	bool ShouldBeRemoved() const;

	V2F GetSize() const override;
	const std::wstring& GetWideTitle() const override;
	std::string GetSlimTitle() const override;
	const std::wstring& GetWideText() const override;
	std::string GetSlimText() const override;

	void SetText(const std::wstring& aText) override;
	void SetText(const std::string& aText) override;
	void SetTitle(const std::wstring& aTitle)override;
	void SetTitle(const std::string& aTitle) override;

private:
	friend class DialogFactory;

	const DialogData* myDialog;
	const std::wstring* myText;
	const std::wstring* myTitle;
	size_t myCurrentIndex;

	void SubscribeToMessages();
	void UnsubscribeToMessages();

	V2F GetTitleSize() const override;
};