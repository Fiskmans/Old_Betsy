
add_executable(fuzz-fiskJson "main.cpp")

SET_TARGET_PROPERTIES(fuzz-fiskJson PROPERTIES FOLDER "fuzzers")
add_dependencies(fuzz-fiskJson Tools)

target_include_directories(fuzz-fiskJson PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(fuzz-fiskJson PUBLIC ${CMAKE_BINARY_DIR})

target_compile_options(fuzz-fiskJson PRIVATE -fsanitize=fuzzer,address)

target_link_libraries(fuzz-fiskJson PUBLIC Tools)


target_link_directories(fuzz-fiskJson PRIVATE "${CLANG_COMPILER_RT}")

target_link_libraries(fuzz-fiskJson PRIVATE clang_rt.fuzzer-x86_64)
target_link_options(fuzz-fiskJson PRIVATE /wholearchive:clang_rt.fuzzer-x86_64.lib)

target_link_libraries(fuzz-fiskJson PRIVATE clang_rt.asan_dynamic-x86_64 clang_rt.asan_dynamic_runtime_thunk-x86_64)
target_link_options(fuzz-fiskJson PRIVATE /wholearchive:clang_rt.asan_dynamic_runtime_thunk-x86_64.lib)

set_target_properties(fuzz-fiskJson
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/fuzzers"
)

file(WRITE ${PROJECT_BINARY_DIR}/runFiskJsonFuzzer.bat "fuzz-fiskJson.exe fiskJson_corpus \necho ISSUES FOUND \npause")
					
add_custom_command(TARGET fuzz-fiskJson POST_BUILD 
					COMMAND ${CMAKE_COMMAND} -E copy_if_different
					${PROJECT_BINARY_DIR}/runFiskJsonFuzzer.bat
					${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/fuzzers)
					
add_custom_command(TARGET fuzz-fiskJson POST_BUILD 
					COMMAND ${CMAKE_COMMAND} -E copy_if_different
					${CLANG_COMPILER_RT}/clang_rt.asan_dynamic-x86_64.dll 
					${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/fuzzers)
					
add_custom_command(TARGET fuzz-fiskJson POST_BUILD 
					COMMAND ${CMAKE_COMMAND} -E make_directory
					${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/fuzzers/fiskJson_corpus)
