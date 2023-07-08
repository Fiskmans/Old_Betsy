
list(APPEND SOURCE_FILES "Demo_game1.h")
list(APPEND SOURCE_FILES "Demo_game1.cpp")

list(APPEND SOURCE_FILES "main.cpp")

project(Demo_game1 VERSION 1.0.0)
list(APPEND SOURCE_FILES "${PROJECT_BINARY_DIR}/config.h")

add_executable(Demo_game1 "${SOURCE_FILES}")

configure_file(config.h.in config.h)
target_include_directories(Demo_game1 PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(Demo_game1 PUBLIC ${CMAKE_BINARY_DIR})

target_link_libraries(Demo_game1 PUBLIC Engine)

set(TARGET_FOLDER $<TARGET_FILE_DIR:Demo_game1>\\data)
set(DATA_FOLDER ${CMAKE_SOURCE_DIR}\\data)

message(target: ${TARGET_FOLDER})
message(data folder: ${DATA_FOLDER})

if(WIN32)
	set(INTERMEDIATE_BAT_FILE ${CMAKE_BINARY_DIR}/generate_symbolic_links.bat)
	set(BAT_FILE $<TARGET_FILE_DIR:Demo_game1>/generate_symbolic_links.bat)
	
	file(TO_NATIVE_PATH ${DATA_FOLDER} DATA_FOLDER_SAFE)
	
	file(WRITE ${INTERMEDIATE_BAT_FILE} "mklink /j data ${DATA_FOLDER_SAFE}")
	add_custom_command(	TARGET Demo_game1 POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E copy_if_different
						${INTERMEDIATE_BAT_FILE} 
						${BAT_FILE})
else()
	add_custom_command(	TARGET Demo_game1 POST_BUILD
						COMMAND ${CMAKE_COMMAND} -E create_symlink
						${DATA_FOLDER} 
						${TARGET_FOLDER})
endif()