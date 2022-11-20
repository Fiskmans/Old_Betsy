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

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(fisk_input)

add_subdirectory(tools)
add_subdirectory(imgui)
add_subdirectory(engine)


#we're defining VCPKG_TARGET_ARCHITECTURE as a workaround for the vs2022 generator not defining VCPKG_TARGET_ARCHITECTURE
set(VCPKG_TARGET_ARCHITECTURE x64)
	add_subdirectory(DirectXTK)
unset(VCPKG_TARGET_ARCHITECTURE)

set(ASSIMP_INSTALL OFF)
set(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(assimp)

if(DEFINED CLANG_COMPILER_RT)
	message("Clang runtime directory specified, enabling fuzzers")
	add_subdirectory(fuzzers)
else()
	message("define CLANG_COMPILER_RT to enable fuzzers")
endif()

Include(FetchContent)

FetchContent_Declare(
	  Catch2
	  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	  GIT_TAG        v3.0.0-preview3)

FetchContent_MakeAvailable(Catch2)

add_subdirectory(unit_tests)

add_subdirectory(demo_game1)

configure_file(versionConfig.h.in versionConfig.h)


