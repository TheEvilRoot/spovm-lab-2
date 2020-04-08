cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_HOME_DIRECTORY}/build)

project(0002)

add_subdirectory(control)
add_subdirectory(handler)

add_custom_target(${PROJECT_NAME} DEPENDS control handler)