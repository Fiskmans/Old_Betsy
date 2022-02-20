
list(APPEND SOURCE_FILES "ExecuteOnDestruct.cpp")
list(APPEND SOURCE_FILES "ExecuteOnDestruct.h")

add_library(Tools STATIC "${SOURCE_FILES}")

project(Tools VERSION 1.0.0)

configure_file(config.h.in config.h)
target_include_directories(Tools PUBLIC "${PROJECT_BINARY_DIR}/..")
target_include_directories(Tools PUBLIC "./..")
