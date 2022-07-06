
#ifndef ENGINE_GAME_ENGINE
#define ENGINE_GAME_ENGINE

#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/RenderScene.h"

#include <string>

namespace engine
{
	class GameEngine;

	class Game
	{
	public:

		virtual std::string AsciiName() = 0;
		virtual std::wstring Name() = 0;

		virtual void Setup() = 0;
		virtual void Update() = 0;
		virtual void PrepareRender() = 0;

		inline void Exit()
		{
			myWantsExit = true;
		}

		inline bool WantsExit()
		{
			return myWantsExit;
		};

	private:
		bool myWantsExit = false;
	};

	class GameEngine : public tools::Singleton<GameEngine>
	{
	public:
		void Init(Game& aGame);

		void RunGame();

		inline Game& GetCurrentGame() { return *myGame; }
		inline RenderScene& GetMainScene() { return myMainScene; }

	private:

		void Imgui();
		void EngineWindow();
		static void PerformanceWindow();

		void Run();
		void Update();
		Game* myGame = nullptr;
		RenderScene myMainScene;
	};
}

#endif