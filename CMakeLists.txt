cmake_minimum_required(VERSION 3.16)

project(MySchedule VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


find_package(Qt5 REQUIRED COMPONENTS Widgets Core)


include_directories(
    ${CMAKE_SOURCE_DIR}/include
)


set(SRC_FILES
    src/main.cpp
    src/Task.cpp
    src/Storage.cpp
    src/MainWindow.cpp
    src/LoginDialog.cpp
    src/RegisterDialog.cpp
    src/AddTaskDialog.cpp
    src/Reminder.cpp
)


set(UI_FILES
    src/MainWindow.ui
    src/LoginDialog.ui
    src/RegisterDialog.ui
    src/AddTaskDialog.ui
)

set(HEADER_FILES
    include/Task.h
    include/Storage.h
    include/MainWindow.h
    include/LoginDialog.h
    include/RegisterDialog.h
    include/AddTaskDialog.h
    include/Reminder.h
)

add_executable(MySchedule
    ${SRC_FILES}
    ${HEADER_FILES}
    ${UI_FILES}
)


target_link_libraries(MySchedule
    PRIVATE
    Qt5::Widgets
    Qt5::Core
)