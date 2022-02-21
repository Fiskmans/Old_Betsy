
list(APPEND SOURCE_FILES "ExecuteOnDestruct.cpp")
list(APPEND SOURCE_FILES "ExecuteOnDestruct.h")
list(APPEND SOURCE_FILES "TimeHelper.cpp")
list(APPEND SOURCE_FILES "TimeHelper.h")
list(APPEND SOURCE_FILES "fiskJSON.h")
list(APPEND SOURCE_FILES "fiskJSON.cpp")
list(APPEND SOURCE_FILES "fiskJSONCustomTypes.h")
list(APPEND SOURCE_FILES "DereferencingIteratorWrapper.h")

add_library(Tools STATIC "${SOURCE_FILES}")

project(Tools VERSION 1.0.0)

configure_file(config.h.in config.h)
target_include_directories(Tools PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(Tools PUBLIC ${CMAKE_BINARY_DIR})
