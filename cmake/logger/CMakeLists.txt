
list(APPEND SOURCE_FILES "Logger.cpp")
list(APPEND SOURCE_FILES "Logger.h")

add_library(Logger STATIC "${SOURCE_FILES}")

project(Logger VERSION 1.0.0)

configure_file(config.h.in config.h)
target_include_directories(Logger PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(Logger PUBLIC ${CMAKE_BINARY_DIR})
