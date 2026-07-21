cmake_minimum_required(VERSION 3.10)

project(myschedule)

set(CMAKE_CXX_STANDARD 11)


add_executable(
    myschedule
    src/main.cpp
    src/task.cpp
)

target_include_directories(
    myschedule
    PRIVATE
    include
)