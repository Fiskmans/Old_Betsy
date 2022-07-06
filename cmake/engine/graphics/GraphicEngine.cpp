
#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/WindowManager.h"
#include "engine/graphics/RenderManager.h"

#include "tools/Stopwatch.h"

#include "logger/Logger.h"

namespace engine
{
	GraphicsEngine::GraphicsEngine()
	{
		myFrameWork = std::make_unique<GraphicsFramework>();
	}

	bool GraphicsEngine::Init(tools::V2ui aWindowSize)
	{
		LOG_SYS_INFO("Initializing Graphics Engine");
		tools::Stopwatch watch;
		{
			tools::TimedScope scopeTimer(watch);
			if (!WindowManager::GetInstance().OpenWindow(aWindowSize))
			{
				LOG_SYS_CRASH("Unable to open window");
				return false;
			}

			if (!myFrameWork->Init())
			{
				LOG_SYS_CRASH("Unable to initialize graphics framework");
				return false;
			}

			if (!RenderManager::GetInstance().Init())
			{
				LOG_SYS_CRASH("Unable to initialize RenderManager");
				return false;
			}
		}
		LOG_SYS_INFO("Graphics Engine initialized in " + std::to_string(watch.Read()) + " seconds");

		return true;
	}

	void GraphicsEngine::BeginFrame(tools::V4f aClearColor)
	{
		RenderManager::GetInstance().BeginFrame(aClearColor);
	}

	void GraphicsEngine::RenderFrame()
	{
		RenderManager::GetInstance().Render();
	}

	void GraphicsEngine::EndFrame()
	{
		RenderManager::GetInstance().EndFrame();
		myFrameWork->EndFrame();
	}

	void GraphicsEngine::Imgui()
	{
		RenderManager::GetInstance().Imgui();
	}
}