cmake_minimum_required(VERSION 3.16)
project(MySchedule VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 自动处理 UIC/MOC/RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 找 Qt5 组件
find_package(Qt5 REQUIRED COMPONENTS Widgets Core)

# 包含头文件目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 收集源文件
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

# 收集 UI 文件（让 AUTOUIC 处理）
set(UI_FILES
    src/MainWindow.ui
    src/LoginDialog.ui
    src/RegisterDialog.ui
    src/AddTaskDialog.ui
)

# 生成可执行文件
add_executable(MySchedule ${SRC_FILES} ${UI_FILES})

# 链接 Qt5 库
target_link_libraries(MySchedule PRIVATE Qt5::Widgets Qt5::Core)