cmake_minimum_required(VERSION 3.14)

set(PROJECT simulation)

project(${PROJECT})

set(SOURCES
    libs/glad.cpp
    src/main.cpp
    src/Window.cpp
)

set_source_files_properties(libs/glad.c PROPERTIES COMPILE_FLAGS -Wno-pedantic)

add_executable(${PROJECT} ${SOURCES})

target_include_directories(${PROJECT} PRIVATE "./libs/include")
target_compile_options(${PROJECT} PRIVATE -O3 -std=c++14 -Wall -Wextra -Wpedantic)
target_link_options(${PROJECT} PRIVATE -lglfw)

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT}>/shaders
)