cmake_minimum_required(VERSION 3.16)
project(task_manager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(external)

add_subdirectory(src)