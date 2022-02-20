add_executable(LAUNCHER main.cpp)

project(LAUNCHER VERSION 1.0.1)

configure_file(config.h.in config.h)
target_include_directories(LAUNCHER PUBLIC "${PROJECT_BINARY_DIR}/..")
target_include_directories(LAUNCHER PUBLIC "./..")
