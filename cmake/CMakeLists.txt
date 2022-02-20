cmake_minimum_required(VERSION 3.22.1)

project(oldBetsy VERSION 1.0.0)

if(DEFINED CUSTOM_CLANG)
	set(CMAKE_C_COMPILER "${CUSTOM_CLANG}clang.exe")
	set(CMAKE_CXX_COMPILER "${CUSTOM_CLANG}clang++.exe")
else()
	set(CMAKE_C_COMPILER clang)
	set(CMAKE_CXX_COMPILER clang++)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(OLD_CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=fuzzer")

option(BUILD_LAUNCHER "build the launcher" ON)

if(BUILD_LAUNCHER)
	add_subdirectory(launcher)
endif()

configure_file(versionConfig.h.in versionConfig.h)


