
list(APPEND SOURCE_FILES "main.cpp")

project(Launcher VERSION 1.0.1)
configure_file(config.h.in config.h)
list(APPEND SOURCE_FILES "${PROJECT_BINARY_DIR}/config.h")

add_executable(Launcher "${SOURCE_FILES}")

target_include_directories(Launcher PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(Launcher PUBLIC ${CMAKE_BINARY_DIR})

target_link_libraries(Launcher PUBLIC Tools)
target_link_libraries(Launcher PUBLIC Logger)