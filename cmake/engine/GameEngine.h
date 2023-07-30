
#ifndef ENGINE_GAME_ENGINE
#define ENGINE_GAME_ENGINE

#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/RenderScene.h"

#include "engine/ImGuiWindow.h"

#include "fisk_input/Input.h"

#include <string>

namespace engine
{
	class GameEngine;

	class Game
	{
	public:
		Game() = default;
		virtual ~Game() = default;

		virtual std::string AsciiName() = 0;
		virtual std::wstring Name() = 0;

		virtual void Setup() = 0;
		virtual void Update() = 0;
		virtual void PrepareRender() = 0;
		virtual std::vector<std::pair<std::reference_wrapper<fisk::input::Action>, std::string>> GetActions() = 0;

		inline void Exit() { myWantsExit = true; }

		inline bool WantsExit() { return myWantsExit; };

	private:
		bool myWantsExit = false;
	};

	class GameEngine : 
		public fisk::tools::Singleton<GameEngine>,
		public engine::ImGuiWindow
	{
	public:
		GameEngine();

		void Init(Game& aGame);

		void RunGame();

		inline Game& GetCurrentGame() { return *myGame; }
		inline RenderScene& GetMainScene() { return myMainScene; }

		void OnImgui() override;
		inline const char* ImGuiName() override { return "Game Engine"; };

	private:
		friend Game;

		void Run();
		void Update();
		Game* myGame = nullptr;
		RenderScene myMainScene;

		fisk::input::Input myInput;
	};
}

#endif