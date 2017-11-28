cmake_minimum_required(VERSION 3.6)
project(Shell)

include_directories(include)
file(GLOB SOURCES "src/*.c")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_executable(shell ${SOURCES})
