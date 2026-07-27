# Schedule Manager

一个基于 C++ 和 Qt 开发的桌面日程管理系统。

本项目实现了任务管理、数据持久化、定时提醒以及图形化交互等功能，为用户提供一个简单易用的个人日程管理工具。

---

## ✨ 项目介绍

Schedule Manager 是一个基于 Qt Widgets 框架开发的桌面应用程序。

用户可以通过图形界面完成：

- 创建任务
- 修改任务
- 删除任务
- 查看任务列表
- 保存任务数据
- 定时提醒

项目采用模块化设计，将：

- 核心业务逻辑
- 数据存储
- 用户界面
- 提醒系统

进行分离，提高代码可维护性和扩展性。

---

## 🛠 技术栈

| 模块 | 技术 |
| --- | --- |
| 编程语言 | C++17 |
| GUI框架 | Qt5 Widgets |
| 构建工具 | CMake |
| 数据存储 | JSON |
| 多线程 | Qt Thread |
| 开发环境 | Linux / Windows |

---

# 📂 项目结构

```
schedule-manager
│
├── include
│   ├── Task.h
│   ├── ScheduleManager.h
│   ├── Storage.h
│   ├── Reminder.h
│   └── ...
│
├── src
│   ├── main.cpp
│   ├── MainWindow.cpp
│   ├── Task.cpp
│   ├── ScheduleManager.cpp
│   ├── Storage.cpp
│   ├── Reminder.cpp
│   └── ...
│
├── sounds
│   └── reminder audio files
│
├── CMakeLists.txt
│
└── README.md
```

---

# 🏗 系统架构

整体结构：

```
              Qt GUI
                 |
                 |
          MainWindow
                 |
     ---------------------
     |                   |
ScheduleManager       Storage
     |
   Task
     |
 Reminder Thread
```

---

# 📌 核心模块

## Task

Task 是系统中的任务数据模型。

负责保存任务基本信息：

- 任务标题
- 任务描述
- 截止时间
- 优先级
- 完成状态


---

## ScheduleManager

ScheduleManager 是核心业务管理模块。

主要负责：

- 添加任务
- 删除任务
- 修改任务
- 查询任务
- 管理任务列表


---

## Storage

Storage 负责数据持久化。

采用 JSON 文件保存任务信息。

例如：

```
data/
└── tasks.json
```

数据格式：

```json
[
    {
        "title": "Meeting",
        "time": "2026-07-27 10:00",
        "finished": false
    }
]
```

---

## Reminder

Reminder 提供任务提醒功能。

通过后台线程定时检查任务：

```
Reminder Thread

        |
        |
  定时检查任务

        |
        |
发现即将开始任务

        |
        |
发送 Qt Signal

        |
        |
GUI显示提醒
```

避免提醒逻辑阻塞主界面。

---

# 🎨 功能展示

## 任务管理

支持：

- 查看任务
- 添加任务
- 编辑任务
- 删除任务


## 数据保存

程序关闭后任务不会丢失。

重新启动程序可以加载之前保存的数据。


## 自动提醒

当任务时间接近时：

- 弹出提醒窗口
- 播放提示音

---

# 🚀 编译运行

## 环境要求

推荐环境：

```
Ubuntu 20.04+
Qt 5.15+
CMake 3.16+
g++ 11+
```

---

## 编译

```bash
git clone https://github.com/jujuboakz/schedule-manager.git

cd schedule-manager

mkdir build

cd build

cmake ..

make
```

---

## 运行

```bash
./MySchedule
```

---

# 🌿 Git开发流程

项目采用模块化分支开发：

```
main

├── feature-core
│
├── feature-storage
│
└── feature-gui
```

不同模块独立开发，最后合并。

---

# 📌 项目特点

- ✅ C++ 面向对象设计
- ✅ Qt 信号槽机制
- ✅ 多线程提醒系统
- ✅ JSON 数据持久化
- ✅ CMake 自动构建
- ✅ 模块化架构设计


---

# 🔮 后续改进

- [ ] 支持任务搜索
- [ ] 支持任务分类
- [ ] 增加优先级排序
- [ ] 支持数据库存储
- [ ] 增加日历视图
- [ ] 支持云同步


---

# 👥 Contributors

jujuboakz


---

# License

MIT License
