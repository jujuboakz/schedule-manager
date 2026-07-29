#!/bin/bash

# ============================================================
# 日程管理系统 - 图形界面测试脚本
# 功能：生成测试用户和测试任务数据，然后启动程序
#       供测试人员通过图形界面验证各项功能
# ============================================================

# 配置
PROGRAM="./build/MySchedule"
TEST_USER="testuser"
TEST_PASS="password"
DATA_DIR="./build/data"

echo "=========================================="
echo "  日程管理器 - 图形界面测试"
echo "=========================================="
echo ""

# 检查可执行文件
if [ ! -f "$PROGRAM" ]; then
    echo "错误: 找不到可执行文件 $PROGRAM"
    echo "请先编译: cd build && make"
    exit 1
fi

# 创建数据目录
mkdir -p "$DATA_DIR"

# ===== 1. 生成测试用户 =====
echo "---- 1. 生成测试用户 ----"
# 密码 "password" 的 SHA-256 哈希值
cat > "$DATA_DIR/users.json" << 'EOF'
{
    "testuser": "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8"
}
EOF
echo "测试用户已创建: 用户名=testuser, 密码=password"

# ===== 2. 生成测试任务 =====
echo "---- 2. 生成测试任务 ----"
# 获取当前日期
TODAY=$(date +%Y-%m-%d)
TOMORROW=$(date -d '+1 day' +%Y-%m-%d)
NOW=$(date +%H:%M:%S)
REMIND_TIME=$(date -d '+1 minute' +%H:%M:%S)

cat > "$DATA_DIR/tasks_testuser.json" << EOF
[
    {
        "id": 1,
        "name": "完成课程设计文档",
        "startTime": "${TODAY}T09:00:00",
        "priority": 0,
        "category": 0,
        "remindTime": "${TODAY}T08:55:00",
        "isCompleted": false
    },
    {
        "id": 2,
        "name": "准备答辩PPT",
        "startTime": "${TODAY}T14:30:00",
        "priority": 0,
        "category": 0,
        "remindTime": "${TODAY}T14:20:00",
        "isCompleted": false
    },
    {
        "id": 3,
        "name": "团队代码审查",
        "startTime": "${TODAY}T16:00:00",
        "priority": 1,
        "category": 2,
        "remindTime": "${TODAY}T15:50:00",
        "isCompleted": false
    },
    {
        "id": 4,
        "name": "阅读《软件工程》第5章",
        "startTime": "${TOMORROW}T10:00:00",
        "priority": 2,
        "category": 0,
        "remindTime": "${TOMORROW}T09:50:00",
        "isCompleted": false
    },
    {
        "id": 5,
        "name": "整理项目笔记",
        "startTime": "${TOMORROW}T15:00:00",
        "priority": 1,
        "category": 2,
        "remindTime": "${TOMORROW}T14:50:00",
        "isCompleted": true
    }
]
EOF
echo "测试任务已生成 (共5条)"

# ===== 3. 显示任务列表 =====
echo "---- 3. 任务列表预览 ----"
cat "$DATA_DIR/tasks_testuser.json" | python3 -m json.tool 2>/dev/null || cat "$DATA_DIR/tasks_testuser.json"
echo ""

# ===== 4. 启动程序 =====
echo "=========================================="
echo "  测试指引"
echo "=========================================="
echo ""
echo "登录信息:"
echo "  用户名: testuser"
echo "  密码:   password"
echo ""
echo "功能验证清单:"
echo "  [ ] 登录成功，进入主界面"
echo "  [ ] 日历显示今天日期高亮"
echo "  [ ] 任务列表显示今天的任务（3条）"
echo "  [ ] 切换日期到明天，显示2条任务（其中1条已完成）"
echo "  [ ] 统计面板显示正确: 总任务5 / 今日3 / 已完成1"
echo "  [ ] 选中任务，点击「切换状态」，状态变化"
echo "  [ ] 点击「添加任务」，输入信息后确认，列表更新"
echo "  [ ] 选中任务，点击「修改任务」，修改后确认，列表更新"
echo "  [ ] 选中任务，点击「删除任务」，确认后列表更新"
echo "  [ ] 等待1-2分钟，任务提醒弹窗出现 + 蜂鸣声"
echo "  [ ] 点击「语音录入」，录音后识别文本自动填入"
echo "  [ ] 点击「刷新列表」，列表和统计面板刷新"
echo ""
echo "=========================================="
echo "  程序启动中..."
echo "=========================================="
sleep 2

# 启动程序
$PROGRAM
