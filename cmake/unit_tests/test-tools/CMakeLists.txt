
add_executable(test-tools main.cpp)

SET_TARGET_PROPERTIES(test-tools PROPERTIES FOLDER "tests")
add_dependencies(test-tools Tools)

target_include_directories(test-tools PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(test-tools PUBLIC ${CMAKE_BINARY_DIR})

target_link_libraries(test-tools PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test-tools PUBLIC Tools)

set_target_properties(test-tools
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/tests"
)