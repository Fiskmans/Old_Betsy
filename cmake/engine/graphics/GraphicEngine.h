#ifndef ENGINE_GRAPHICS_GRAPHICSENGINE_H
#define ENGINE_GRAPHICS_GRAPHICSENGINE_H

#include "engine/graphics/GraphicsFramework.h"

#include "tools/MathVector.h"
#include "tools/Singleton.h"

#include <memory>

namespace engine
{
	class GraphicsEngine : public tools::Singleton<GraphicsEngine>
	{
	public:
		GraphicsEngine();
		~GraphicsEngine() = default;

		bool Init(tools::V2ui aWindowSize = tools::V2ui(0, 0));
		void BeginFrame(tools::V4f aClearcolor);
		void RenderFrame();
		void EndFrame();

		inline GraphicsFramework& GetFrameWork() { return *myFrameWork; }

		void Imgui();

	private:

		std::unique_ptr<GraphicsFramework> myFrameWork = nullptr;
	};

}

#endif