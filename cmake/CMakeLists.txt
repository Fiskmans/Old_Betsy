cmake_minimum_required(VERSION 3.22.1)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(DEFINED CUSTOM_CLANG)
	set(CMAKE_C_COMPILER "${CUSTOM_CLANG}clang.exe")
	set(CMAKE_CXX_COMPILER "${CUSTOM_CLANG}clang++.exe")
	set(CMAKE_LINK_EXECUTABLE "${CUSTOM_CLANG}llvm-link.exe")
endif()

message(STATUS "using c   compiler: ${CMAKE_C_COMPILER}")
message(STATUS "using c++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "using       linker: ${CMAKE_CXX_LINK_EXECUTABLE}")

project(oldBetsy VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)


set(TEST_VARIABLE testus_testus)

add_subdirectory(tools)
add_subdirectory(imgui)
add_subdirectory(launcher)

if(DEFINED CLANG_COMPILER_RT)
	add_subdirectory(fuzzers)
endif()

configure_file(versionConfig.h.in versionConfig.h)


