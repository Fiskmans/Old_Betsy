#ifndef DEMO_GAME1_DEMO_GAME1_H
#define DEMO_GAME1_DEMO_GAME1_H

#include "engine/GameEngine.h"

class Demo_game1 final : public engine::Game
{
public:

	std::string AsciiName() final
	{
		return "Demo game";
	}

	std::wstring Name() final
	{
		return L"Demo game";
	}

	void Setup() final;
	void Update() final;
	void PrepareRender() final;

private:

};

#endif