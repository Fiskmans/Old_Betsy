
list(APPEND SOURCE_FILES "graphics/GraphicEngine.cpp" 			"graphics/GraphicEngine.h")
list(APPEND SOURCE_FILES "graphics/WindowManager.cpp" 			"graphics/WindowManager.h")
list(APPEND SOURCE_FILES "graphics/GraphicsFramework.cpp" 		"graphics/GraphicsFramework.h")
list(APPEND SOURCE_FILES "graphics/RenderManager.cpp" 			"graphics/RenderManager.h")
list(APPEND SOURCE_FILES "graphics/GBuffer.cpp"					"graphics/GBuffer.h")
list(APPEND SOURCE_FILES "graphics/Texture.cpp"					"graphics/Texture.h")
list(APPEND SOURCE_FILES "graphics/TextureFactory.cpp"			"graphics/TextureFactory.h")
list(APPEND SOURCE_FILES "graphics/FullscreenRenderer.cpp"		"graphics/FullscreenRenderer.h")
list(APPEND SOURCE_FILES "graphics/RenderStateManager.cpp"		"graphics/RenderStateManager.h")
list(APPEND SOURCE_FILES "graphics/RenderScene.cpp"				"graphics/RenderScene.h")
list(APPEND SOURCE_FILES "graphics/Camera.cpp"					"graphics/Camera.h")
list(APPEND SOURCE_FILES "graphics/ShaderBuffers.h")
list(APPEND SOURCE_FILES "graphics/ShaderMappings.h")
list(APPEND SOURCE_FILES "graphics/EnvironmentLight.h")
list(APPEND SOURCE_FILES "graphics/BoneBuffer.h")
list(APPEND SOURCE_FILES "graphics/Releasable.h")

list(APPEND SOURCE_FILES "assets/ShaderCompiler.cpp"			"assets/ShaderCompiler.h")
list(APPEND SOURCE_FILES "assets/ShaderTypes.cpp"				"assets/ShaderTypes.h")
list(APPEND SOURCE_FILES "assets/Asset.cpp"						"assets/Asset.h")
list(APPEND SOURCE_FILES "assets/AssetManager.cpp"				"assets/AssetManager.h")
list(APPEND SOURCE_FILES "assets/ModelInstance.cpp"				"assets/ModelInstance.h")
list(APPEND SOURCE_FILES "assets/Model.cpp"						"assets/Model.h")
list(APPEND SOURCE_FILES "assets/AnimationData.cpp"				"assets/AnimationData.h")
list(APPEND SOURCE_FILES "assets/TextureLoader.cpp"				"assets/TextureLoader.h")
list(APPEND SOURCE_FILES "assets/ModelLoader.cpp"				"assets/ModelLoader.h")
list(APPEND SOURCE_FILES "assets/ShaderFlags.h")
list(APPEND SOURCE_FILES "assets/PointLight.h")

list(APPEND SOURCE_FILES "GameEngine.cpp" 						"GameEngine.h")
list(APPEND SOURCE_FILES "ImGuiManager.cpp" 					"ImGuiManager.h")
list(APPEND SOURCE_FILES "ImGuiWindow.cpp" 						"ImGuiWindow.h")
list(APPEND SOURCE_FILES "SettingsManager.cpp" 					"SettingsManager.h")
list(APPEND SOURCE_FILES "Time.cpp" 							"Time.h")

file(READ Version.txt VERSION_FILE)
string(REGEX MATCH "VERSION_MAJOR ([0-9]+).*VERSION_MINOR ([0-9]+).*VERSION_PATCH ([0-9]+)" _ ${VERSION_FILE})

set(VERSION_MAJOR ${CMAKE_MATCH_1}) 
set(VERSION_MINOR ${CMAKE_MATCH_2}) 
set(VERSION_PATCH ${CMAKE_MATCH_3})

set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

message("Engine is on version ${VERSION_STRING}")

project(Engine VERSION ${VERSION_STRING})
list(APPEND SOURCE_FILES "${PROJECT_BINARY_DIR}/config.h")

add_library(Engine "${SOURCE_FILES}")

target_precompile_headers(Engine PUBLIC "pch.h")

if(WIN32)
	add_custom_command(TARGET Engine 
	POST_BUILD 
	COMMAND 
		${CMAKE_SOURCE_DIR}/engine/IncrementPatch.bat
		${CMAKE_SOURCE_DIR}/engine/Version.txt)
endif()

configure_file(config.h.in config.h)
target_include_directories(Engine PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(Engine PUBLIC ${CMAKE_BINARY_DIR})
target_include_directories(Engine PUBLIC ${CMAKE_SOURCE_DIR}/DirectxTK/inc/)
target_include_directories(Engine PUBLIC ${CMAKE_SOURCE_DIR}/assimp/include/)
target_include_directories(Engine PUBLIC ${CMAKE_BINARY_DIR}/assimp/include/)

target_link_libraries(Engine PUBLIC Tools)
target_link_libraries(Engine PUBLIC Imgui)

target_link_libraries(Engine PUBLIC DirectXTK)
target_link_libraries(Engine PUBLIC assimp)
target_link_libraries(Engine PUBLIC d3d11.lib)

target_link_libraries(Engine PUBLIC fisk_tools)
target_link_libraries(Engine PUBLIC fisk_input)
