#!/bin/bash

# ============================================
# 读写并发压力测试
# 同时测试：批量写入 + 频繁读取
# ============================================

PROGRAM="./build/MySchedule"
USERNAME="tester"
PASSWORD="123456"
COUNT=${1:-100}          # 添加任务数量
READ_INTERVAL=${2:-10}   # 每添加 N 个任务读取一次

# 设置库路径
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./vosk-api/lib

echo "=========================================="
echo "   📊 读写并发压力测试"
echo "   将批量添加 $COUNT 个任务"
echo "   每 $READ_INTERVAL 个任务读取一次"
echo "=========================================="

# 清理旧数据
rm -f build/data/tasks_tester.json

# 预注册用户
$PROGRAM $USERNAME $PASSWORD showtask > /dev/null 2>&1

START_TIME=$(date +%s)
WRITE_SUCCESS=0
WRITE_FAIL=0
READ_SUCCESS=0
READ_FAIL=0

echo ""
echo "开始读写并发测试..."
echo ""

for i in $(seq 1 $COUNT); do
    TASK_NAME="RW测试任务_$i"
    START_DATE="2026-07-29"
    HOUR=$((8 + i % 12))
    MINUTE=$((i % 60))
    SECOND=$((i % 60))
    START_TIME_STR="$START_DATE $(printf "%02d" $HOUR):$(printf "%02d" $MINUTE):$(printf "%02d" $SECOND)"
    REMIND_TIME_STR="$START_DATE $(printf "%02d" $HOUR):$(printf "%02d" $MINUTE):$(printf "%02d" $SECOND)"
    PRIORITY="中"
    CATEGORY="生活"
    
    # ===== 写入操作 =====
    if $PROGRAM $USERNAME $PASSWORD addtask "$TASK_NAME" "$START_TIME_STR" "$PRIORITY" "$CATEGORY" "$REMIND_TIME_STR" >> /tmp/rw_test.log 2>&1; then
        WRITE_SUCCESS=$((WRITE_SUCCESS + 1))
    else
        WRITE_FAIL=$((WRITE_FAIL + 1))
        echo "   [✗] 写入失败: 第 $i 个任务"
    fi
    
    # ===== 每隔 N 个任务执行一次读取 =====
    if [ $((i % READ_INTERVAL)) -eq 0 ]; then
        echo "   [进度] 已写入 $i/$COUNT，执行读取验证..."
        
        # 读取测试1：showtask 显示所有任务
        if $PROGRAM $USERNAME $PASSWORD showtask > /tmp/rw_read_output.log 2>&1; then
            READ_SUCCESS=$((READ_SUCCESS + 1))
            
            # 验证读取到的任务数量是否正确
            READ_COUNT=$(grep -c "ID:" /tmp/rw_read_output.log)
            FILE_COUNT=$(grep -c '"id"' build/data/tasks_tester.json 2>/dev/null || echo "0")
            echo "   [读取] 文件中有 $FILE_COUNT 个任务，showtask 显示 $READ_COUNT 个任务"
            
            if [ "$READ_COUNT" -eq "$FILE_COUNT" ]; then
                echo "   [✓] 读写数据一致！"
            else
                echo "   [✗] 读写数据不一致！文件: $FILE_COUNT，显示: $READ_COUNT"
            fi
        else
            READ_FAIL=$((READ_FAIL + 1))
            echo "   [✗] 读取失败！"
        fi
    fi
done

END_TIME=$(date +%s)
ELAPSED=$((END_TIME - START_TIME))

echo ""
echo "=========================================="
echo "   ✅ 读写并发压力测试完成！"
echo "=========================================="
echo ""
echo "   📝 写入结果:"
echo "       成功: $WRITE_SUCCESS"
echo "       失败: $WRITE_FAIL"
echo ""
echo "   📖 读取结果:"
echo "       成功: $READ_SUCCESS"
echo "       失败: $READ_FAIL"
echo ""
echo "   ⏱️  总耗时: $ELAPSED 秒"
echo "=========================================="

# 最终验证
echo ""
echo "📋 最终数据验证:"
cd build
TOTAL=$(grep -c '"id"' data/tasks_tester.json 2>/dev/null || echo "0")
echo "   文件中的任务数: $TOTAL"

if [ $TOTAL -eq $COUNT ]; then
    echo "   ✅ 数据完整！所有 $COUNT 个任务已保存。"
else
    echo "   ⚠️ 预期 $COUNT 个，实际 $TOTAL 个"
fi

echo ""
echo "=========================================="
echo "   日志文件: /tmp/rw_test.log"
echo "=========================================="
