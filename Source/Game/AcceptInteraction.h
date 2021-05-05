#pragma once
#include "Publisher.hpp"

class Button;
class Scene;
class TextInstance;
class TextFactory;

class AcceptInteraction: public Publisher
{
public:
	AcceptInteraction();
	~AcceptInteraction();

	bool UpdateAcceptScreen();
	void SetScenePtr(Scene* aScene);
	void SetTextFactory(TextFactory* aTextFactory);

	void InitializeInteraction(std::string aTextToDisplay, bool aAcceptOption, bool aCancelOption);
	void EndInteraction();

private:

	bool myInteractionActive;

	Scene* myScene;
	TextFactory* myTextFactory;

	Button* myAcceptButton;
	Button* myCancelButton;
	TextInstance* myText;
};