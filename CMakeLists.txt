cmake_minimum_required(VERSION 3.16)
project(pong C)

set(CMAKE_C_STANDARD 99)

#include_directories(/usr/local/Cellar/json-c/0.15/include/json-c)
#find_package(json-c REQUIRED)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

#find_package (Python COMPONENTS Interpreter Development)
#include_directories(${Python_INCLUDE_DIRS})

add_executable(pong main.c)

target_link_libraries(pong ${CURSES_LIBRARIES})
#target_link_libraries(pong ${CURSES_LIBRARIES} ${JSON-C_LIBRARIES})
