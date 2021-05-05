#include "pch.h"
#include "DialogFactory.h"
#include "SpriteFactory.h"


DialogFactory::DialogFactory() :
	mySpriteFactoryPtr(nullptr)
{
}

DialogFactory::~DialogFactory()
{
	mySpriteFactoryPtr = nullptr;

	UnSubscribeToMessage(MessageType::UpdateTime);
}

bool DialogFactory::Init(SpriteRenderer* aRenderer, SpriteFactory* aSpriteFactory)
{
	mySpriteFactoryPtr = aSpriteFactory;

	DialogInstance::ourBackgroundRendererPtr = aRenderer;

	SubscribeToMessage(MessageType::UpdateTime);

	return true;
}

DialogInstance* DialogFactory::CreateGameDialog(const std::string& aFilePath, const V2F& aBuffer, const std::string& aBackgroundPath, const std::string& aFontPath)
{
	DialogInstance* inst = new DialogInstance();

	inst->Init(GetFont(aFontPath), GetDialog(aFilePath), aBuffer, mySpriteFactoryPtr->CreateSprite(aBackgroundPath));
	inst->SetPosition({ 0.5f, 0.9f });
	inst->SetPivot({ 0.5f, 1.f });
	inst->SetColor(V4F(0, 0, 0, 1));
	inst->SetTitleColor(V4F(0, 0, 0, 1));

	return inst;
}

void DialogFactory::RecieveMessage(const Message& aMessage)
{
	if (aMessage.myMessageType == MessageType::UpdateTime)
	{
		myCurrentDay = aMessage.myIntValue2;
	}
}

bool DialogFactory::LoadDialog(const std::string& aFilePath)
{
	FiskJSON::Object dialogDoc;

	dialogDoc.Parse(Tools::ReadWholeFile(aFilePath));

	myLoadedDialogs[aFilePath] = DialogInstance::DialogData();

	std::string val;
	std::string toWStringStorage;
	size_t pos;

	if (dialogDoc.Has("Daily"))
	{
		myLoadedDialogs[aFilePath].isDaily = dialogDoc["Daily"];
	}

	myLoadedDialogs[aFilePath].lastTriggered = 0;
	myLoadedDialogs[aFilePath].currentIndex = -1;

	for (auto& i : dialogDoc["Dialogues"].Get<FiskJSON::Array>())
	{
		myLoadedDialogs[aFilePath].data.push_back(std::vector<DialogInstance::DialogData::Snippet>());
		std::vector<DialogInstance::DialogData::Snippet>& dialog = myLoadedDialogs[aFilePath].data.back();

		for (auto& j : (*i).Get<FiskJSON::Array>())
		{
			if (j->GetIf(val))
			{
				dialog.push_back(DialogInstance::DialogData::Snippet());
				pos = val.find("#");

				toWStringStorage = val.substr(0, pos);
				dialog.back().speaker = std::wstring(toWStringStorage.begin(), toWStringStorage.end());

				toWStringStorage = val.substr(pos + 1);
				dialog.back().text = std::wstring(toWStringStorage.begin(), toWStringStorage.end());

				pos = dialog.back().text.find(L"\\n");

				while (pos != std::string::npos)
				{
					dialog.back().text.replace(dialog.back().text.begin() + pos, dialog.back().text.begin() + pos + 2, L"\n");
					pos = dialog.back().text.find(L"\\n");
				}
			}
			else
			{
				SYSERROR("Json for dialog is faulty ", aFilePath);
				myLoadedDialogs.erase(aFilePath);
				return false;
			}
		}
#ifndef _RETAIL
		myLoadedDialogs[aFilePath].filePath = aFilePath;
#endif // !_RETAIL
	}


	return true;
}

const DialogInstance::DialogData* DialogFactory::GetDialog(const std::string& aFilePath)
{
	if (myLoadedDialogs.find(aFilePath) == myLoadedDialogs.end())
	{
		if (!LoadDialog(aFilePath))
		{
			SYSERROR("Could not load dialog ", aFilePath);

			return nullptr;
		}
	}

	if (!myLoadedDialogs[aFilePath].isDaily || myLoadedDialogs[aFilePath].lastTriggered < myCurrentDay)
	{
		myLoadedDialogs[aFilePath].lastTriggered = myCurrentDay;

		myLoadedDialogs[aFilePath].currentIndex++;

		if (myLoadedDialogs[aFilePath].currentIndex >= myLoadedDialogs[aFilePath].data.size())
		{
			myLoadedDialogs[aFilePath].currentIndex = myLoadedDialogs[aFilePath].data.size() - 1;
		}
	}
	else if (myLoadedDialogs[aFilePath].isDaily && myLoadedDialogs[aFilePath].currentIndex < myLoadedDialogs[aFilePath].data.size() - 1)
	{
		static DialogInstance::DialogData d;
		if (d.data.size() < 1)
		{
			d.data.push_back(std::vector<DialogInstance::DialogData::Snippet>());
			d.data.back().push_back({ L"Ruth", L"I'll come back and check later" });
		}

		return &d;
	}

	Message mess;
	mess.myIntValue = myLoadedDialogs[aFilePath].currentIndex;
	mess.myText = aFilePath;
	mess.myMessageType = MessageType::DialogueStarted;
	PostMaster::GetInstance()->SendMessages(mess);

	return &myLoadedDialogs[aFilePath];
}
