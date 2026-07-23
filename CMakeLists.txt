cmake_minimum_required(VERSION 3.10)

project(myschedule)

set(CMAKE_CXX_STANDARD 11)


add_executable(
    myschedule
    src/main.cpp
    src/Task.cpp
    src/ScheduleManager.cpp
    src/Storage.cpp
    src/User.cpp
    src/CommandParser.cpp
)

target_include_directories(
    myschedule
    PRIVATE
    include
)