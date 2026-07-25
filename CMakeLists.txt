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
    ${CMAKE_SOURCE_DIR}/vosk-api/include
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
    src/VoiceRecognizer.cpp
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
    include/VoiceRecognizer.h
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

    # 1. 告诉链接器去 vosk-api/lib 目录里找文件
    -L${CMAKE_SOURCE_DIR}/vosk-api/lib
    
    # 2. 链接 vosk 动态库 (不用写 lib 和 .so，写 vosk 即可)
    -lvosk
    
    # 3. (重要) 为了在运行时能找到 libvosk.so，把库路径加入 RPATH
    -Wl,-rpath,${CMAKE_SOURCE_DIR}/vosk-api/lib
)