
list(APPEND SOURCE_FILES "main.cpp")

project(Launcher VERSION 1.0.1)
configure_file(config.h.in config.h)
list(APPEND SOURCE_FILES "${PROJECT_BINARY_DIR}/config.h")

add_executable(Launcher "${SOURCE_FILES}")

target_include_directories(Launcher PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(Launcher PUBLIC ${CMAKE_BINARY_DIR})

target_link_libraries(Launcher PUBLIC Tools)
target_link_libraries(Launcher PUBLIC Logger)
target_link_libraries(Launcher PUBLIC Engine)
target_link_libraries(Launcher PUBLIC Imgui)
target_link_libraries(Launcher PUBLIC Demo_game1)

target_link_libraries(Launcher PUBLIC d3d11.lib) 

set(TARGET_FOLDER $<TARGET_FILE_DIR:Launcher>\\data)
set(DATA_FOLDER ${CMAKE_SOURCE_DIR}\\data)

message(target: ${TARGET_FOLDER})
message(data folder: ${DATA_FOLDER})

if(WIN32)
	set(INTERMEDIATE_BAT_FILE ${CMAKE_BINARY_DIR}/generate_symbolic_links.bat)
	set(BAT_FILE $<TARGET_FILE_DIR:Launcher>/generate_symbolic_links.bat)
	
	file(TO_NATIVE_PATH ${DATA_FOLDER} DATA_FOLDER_SAFE)
	
	file(WRITE ${INTERMEDIATE_BAT_FILE} "mklink /j data ${DATA_FOLDER_SAFE}")
	add_custom_command(	TARGET Launcher POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E copy_if_different
						${INTERMEDIATE_BAT_FILE} 
						${BAT_FILE})
else()
	add_custom_command(	TARGET Launcher POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E create_symlink
						${DATA_FOLDER} 
						${TARGET_FOLDER})
endif()
				   