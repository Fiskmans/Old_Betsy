
list(APPEND SOURCE_FILES "Demo_game1.h")
list(APPEND SOURCE_FILES "Demo_game1.cpp")

project(Demo_game1 VERSION 1.0.0)
list(APPEND SOURCE_FILES "${PROJECT_BINARY_DIR}/config.h")

add_library(Demo_game1 "${SOURCE_FILES}")

configure_file(config.h.in config.h)
target_include_directories(Demo_game1 PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(Demo_game1 PUBLIC ${CMAKE_BINARY_DIR})