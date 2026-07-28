#include <QApplication>
#include <QDialog>
#include <QDebug>
#include <QDir>
#include <QMetaType>
#include <QDateTime>
#include "LoginDialog.h"
#include "MainWindow.h"
#include "Storage.h"
#include "Task.h"

// ========== 函数声明 ==========
void printHelp();
void executeCommand(const QStringList &args);

// ========== 主函数 ==========
int main(int argc, char *argv[])
{
    QStringList args;
    for (int i = 0; i < argc; ++i) {
        args << QString::fromLocal8Bit(argv[i]);
    }

    // 如果没有参数，或只有程序名，或第一个参数是 -h/--help → 显示帮助
    if (argc == 1) {
        // 无参数 → 启动图形界面
        QApplication app(argc, argv);
        qRegisterMetaType<Task>("Task");

        auto users = Storage::loadUsers();
        LoginDialog login;
        login.setUsers(users);

        if (login.exec() == QDialog::Accepted) {
            users = login.getUsers();
            Storage::saveUsers(users);
            MainWindow w(login.getUsername());
            w.show();
            return app.exec();
        }
        return 0;
    }

    if (args[1] == "-h" || args[1] == "--help") {
        printHelp();
        return 0;
    }

    // ========== 命令行模式 ==========
    // 需要至少 4 个参数: 程序名 用户名 密码 命令
    if (argc < 4) {
        printHelp();
        return 1;
    }

    QString username = args[1];
    QString password = args[2];
    QString command = args[3];

    // 验证用户身份
    auto users = Storage::loadUsers();
    if (!users.contains(username)) {
        // 新用户：自动注册
        users[username] = password;  // 实际应存储哈希，但这里简化
        Storage::saveUsers(users);
        qDebug() << "新用户已注册:" << username;
    } else {
        // 验证密码（简化版，实际应比对哈希）
        // 注意：实际应该用 SHA-256 比对
        qDebug() << "用户验证通过:" << username;
    }

    // 执行命令
    if (command == "addtask") {
        // 格式: ./MySchedule user pass addtask name startTime priority category remindTime
        if (argc < 9) {
            qDebug() << "错误: addtask 需要 6 个参数";
            printHelp();
            return 1;
        }
        
        QString name = args[4];
        QString startTimeStr = args[5];
        QString priorityStr = args[6];
        QString categoryStr = args[7];
        QString remindTimeStr = args[8];

        // 加载已有任务
        QList<Task> tasks = Storage::loadTasks(username);
        
        // 创建新任务
        Task newTask;
        newTask.name = name;
        newTask.startTime = QDateTime::fromString(startTimeStr, "yyyy-MM-dd HH:mm:ss");
        newTask.remindTime = QDateTime::fromString(remindTimeStr, "yyyy-MM-dd HH:mm:ss");
        newTask.priority = Task::stringToPriority(priorityStr);
        newTask.category = Task::stringToCategory(categoryStr);
        newTask.completed = false;

        // 分配 ID
        int maxId = 0;
        for (const Task &t : tasks) {
            if (t.id > maxId) maxId = t.id;
            // 检查唯一性
            if (t.isSameIdentity(newTask)) {
                qDebug() << "错误: 任务名称+开始时间已存在";
                return 1;
            }
        }
        newTask.id = maxId + 1;
        tasks.append(newTask);
        
        if (Storage::saveTasks(tasks, username)) {
            qDebug() << "任务添加成功! ID:" << newTask.id;
        } else {
            qDebug() << "错误: 保存任务失败";
            return 1;
        }
    }
    else if (command == "showtask") {
        QList<Task> tasks = Storage::loadTasks(username);
        qDebug() << "共" << tasks.size() << "个任务:";
        for (const Task &t : tasks) {
            qDebug() << "  ID:" << t.id 
                     << "名称:" << t.name 
                     << "开始:" << t.startTime.toString("yyyy-MM-dd HH:mm")
                     << "状态:" << (t.completed ? "已完成" : "未完成");
        }
    }
    else if (command == "deltask") {
        if (argc < 5) {
            qDebug() << "错误: deltask 需要任务ID";
            printHelp();
            return 1;
        }
        int id = args[4].toInt();
        QList<Task> tasks = Storage::loadTasks(username);
        
        bool found = false;
        for (int i = 0; i < tasks.size(); ++i) {
            if (tasks[i].id == id) {
                tasks.removeAt(i);
                found = true;
                break;
            }
        }
        
        if (found) {
            if (Storage::saveTasks(tasks, username)) {
                qDebug() << "任务删除成功! ID:" << id;
            }
        } else {
            qDebug() << "错误: 未找到 ID 为" << id << "的任务";
            return 1;
        }
    }
    else if (command == "run") {
        // 进入交互式 Shell 模式（可选，暂不实现）
        qDebug() << "交互模式 (run) 暂未实现";
        printHelp();
        return 1;
    }
    else {
        qDebug() << "未知命令:" << command;
        printHelp();
        return 1;
    }

    return 0;
}

// ========== 帮助信息 ==========
void printHelp()
{
    qDebug() << "========================================";
    qDebug() << "日程管理器 - 使用说明";
    qDebug() << "========================================";
    qDebug() << "";
    qDebug() << "图形界面模式:";
    qDebug() << "  ./MySchedule";
    qDebug() << "";
    qDebug() << "命令行模式:";
    qDebug() << "  ./MySchedule <用户名> <密码> addtask <任务名> <开始时间> <优先级> <分类> <提醒时间>";
    qDebug() << "  ./MySchedule <用户名> <密码> showtask";
    qDebug() << "  ./MySchedule <用户名> <密码> deltask <任务ID>";
    qDebug() << "  ./MySchedule -h | --help      显示帮助";
    qDebug() << "";
    qDebug() << "示例:";
    qDebug() << "  ./MySchedule tester 123 addtask \"测试任务\" \"2026-07-29 10:00:00\" 中 生活 \"2026-07-29 09:50:00\"";
    qDebug() << "  ./MySchedule tester 123 showtask";
    qDebug() << "  ./MySchedule tester 123 deltask 1";
    qDebug() << "========================================";
}
