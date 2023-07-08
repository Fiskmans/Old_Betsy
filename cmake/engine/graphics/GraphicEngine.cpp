
#include "engine/graphics/GraphicEngine.h"
#include "engine/graphics/WindowManager.h"
#include "engine/graphics/RenderManager.h"

#include "engine/utilities/Stopwatch.h"

#include "tools/Logger.h"

namespace engine::graphics
{
	GraphicsEngine::GraphicsEngine()
	{
		myFrameWork = std::make_unique<GraphicsFramework>();
	}

	bool GraphicsEngine::Init(tools::V2ui aWindowSize)
	{
		LOG_SYS_INFO("Initializing Graphics Engine");
		engine::utilities::StopWatch watch;
		{
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

			if (!ModelInstance::InitShared())
			{
				LOG_SYS_CRASH("Failed to initialize modelinstance shared resources");
				return false;
			}
		}
		LOG_SYS_INFO("Graphics Engine initialized in " + std::to_string(watch.Stop().count()) + " seconds");

		return true;
	}

	void GraphicsEngine::RenderFrame()
	{
		RenderManager::GetInstance().Render();
	}

	void GraphicsEngine::EndFrame()
	{
		myFrameWork->EndFrame();
	}

	void GraphicsEngine::Imgui()
	{
		RenderManager::GetInstance().Imgui();
	}
}