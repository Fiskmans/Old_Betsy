
add_executable(test-fiskJson main.cpp)

SET_TARGET_PROPERTIES(test-fiskJson PROPERTIES FOLDER "tests")
add_dependencies(test-fiskJson Tools)

target_include_directories(test-fiskJson PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(test-fiskJson PUBLIC ${CMAKE_BINARY_DIR})

target_link_libraries(test-fiskJson PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test-fiskJson PUBLIC Tools)

set_target_properties(test-fiskJson
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/tests"
)