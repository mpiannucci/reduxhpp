cmake_minimum_required(VERSION 3.0)
project(redux_examples)

# Compiler flags
if (MSVC)
    # Add /SUBSYSTEM:WINDOWS for GUI
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++1y")
endif ()

# Commond Dependencies 
set(REDUX_HPP_DIR ${CMAKE_SOURCE_DIR}/../)
include_directories(${REDUX_HPP_DIR}/include)

# Add all of the examples
add_subdirectory(counter)