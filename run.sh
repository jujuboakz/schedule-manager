#!/bin/bash

# 获取脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# 切换到 build 目录
cd "$SCRIPT_DIR/build"

# 设置库路径
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../vosk-api/lib

# 运行程序
./MySchedule "$@"
