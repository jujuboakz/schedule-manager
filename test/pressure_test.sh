#!/bin/bash

# 压力测试脚本 - 直接使用 MySchedule（不依赖 run.sh）
PROGRAM="./build/MySchedule"
USERNAME="tester"
PASSWORD="123456"
COUNT=${1:-100}

# 设置库路径（关键！）
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./vosk-api/lib

echo "=========================================="
echo "   📊 压力测试开始"
echo "   将批量添加 $COUNT 个任务"
echo "=========================================="

# 测试用户是否存在（自动注册）
$PROGRAM $USERNAME $PASSWORD showtask > /dev/null 2>&1

START_TIME=$(date +%s)
SUCCESS=0
FAIL=0

echo ""
echo "开始添加 $COUNT 个任务..."
echo ""

for i in $(seq 1 $COUNT); do
    TASK_NAME="压力测试任务_$i"
    START_DATE="2026-07-29"
    HOUR=$((8 + i % 12))
    MINUTE=$((i % 60))
    SECOND=$((i % 60))
    START_TIME_STR="$START_DATE $(printf "%02d" $HOUR):$(printf "%02d" $MINUTE):$(printf "%02d" $SECOND)"
    REMIND_TIME_STR="$START_DATE $(printf "%02d" $HOUR):$(printf "%02d" $MINUTE):$(printf "%02d" $SECOND)"
    PRIORITY="中"
    CATEGORY="生活"
    
    if [ $((i % 10)) -eq 0 ]; then
        echo "进度: $i/$COUNT"
    fi
    
    # 直接调用 MySchedule，错误输出到日志
    if $PROGRAM $USERNAME $PASSWORD addtask "$TASK_NAME" "$START_TIME_STR" "$PRIORITY" "$CATEGORY" "$REMIND_TIME_STR" >> /tmp/pressure_test.log 2>&1; then
        SUCCESS=$((SUCCESS + 1))
    else
        FAIL=$((FAIL + 1))
        echo "  警告: 第 $i 个任务添加失败"
    fi
done

END_TIME=$(date +%s)
ELAPSED=$((END_TIME - START_TIME))

echo ""
echo "=========================================="
echo "   ✅ 压力测试完成！"
echo "=========================================="
echo "   请求总数: $COUNT"
echo "   成功添加: $SUCCESS"
echo "   添加失败: $FAIL"
echo "   总耗时: $ELAPSED 秒"
echo "=========================================="

# 验证数据
echo ""
echo "📋 验证任务数据:"
cd build
TASK_COUNT=$(grep -c '"id"' data/tasks_$USERNAME.json 2>/dev/null || echo "0")
echo "   文件中的任务数: $TASK_COUNT"

if [ $TASK_COUNT -eq $COUNT ]; then
    echo "   ✅ 数据验证通过！"
else
    echo "   ⚠️ 预期 $COUNT 个，实际 $TASK_COUNT 个"
fi
echo "=========================================="
