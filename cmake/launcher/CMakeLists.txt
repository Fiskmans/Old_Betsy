
list(APPEND SOURCE_FILES "main.cpp")

add_executable(Launcher "${SOURCE_FILES}")

project(Launcher VERSION 1.0.1)

configure_file(config.h.in config.h)
target_include_directories(Launcher PUBLIC "${PROJECT_BINARY_DIR}/..")
target_include_directories(Launcher PUBLIC "./..")
