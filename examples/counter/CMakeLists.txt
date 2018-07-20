cmake_minimum_required(VERSION 3.0)
project(redux_counter)

set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})