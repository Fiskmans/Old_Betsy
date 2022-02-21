list(APPEND SOURCE_FILES "imgui.cpp")
list(APPEND SOURCE_FILES "imgui.h")
list(APPEND SOURCE_FILES "imgui_demo.cpp")
list(APPEND SOURCE_FILES "imgui_draw.cpp")
list(APPEND SOURCE_FILES "imgui_internal.h")
list(APPEND SOURCE_FILES "imgui_tables.cpp")
list(APPEND SOURCE_FILES "imgui_widgets.cpp")
list(APPEND SOURCE_FILES "imstb_rectpack.h")
list(APPEND SOURCE_FILES "imstb_textedit.h")
list(APPEND SOURCE_FILES "imstb_truetype.h")

list(APPEND SOURCE_FILES "backend/imgui_impl_dx11.cpp")
list(APPEND SOURCE_FILES "backend/imgui_impl_dx11.h")
list(APPEND SOURCE_FILES "backend/imgui_impl_win32.cpp")
list(APPEND SOURCE_FILES "backend/imgui_impl_win32.h")

list(APPEND SOURCE_FILES "WindowControl.cpp")
list(APPEND SOURCE_FILES "WindowControl.h")

add_library(Imgui STATIC "${SOURCE_FILES}")

project(Imgui VERSION 1.0.0)

configure_file(config.h.in config.h)

target_compile_options(Imgui PRIVATE -D_CRT_SECURE_NO_WARNINGS)
target_include_directories(Imgui PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(Imgui PUBLIC ${CMAKE_BINARY_DIR})
