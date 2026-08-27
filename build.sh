#!/bin/bash

# 【修改点 1：将路径替换为你的 i.MX6ULL 工具链的真实绝对路径】
# 注意：以下路径仅为 100ask SDK 的示例路径，请根据你 Ubuntu 里的实际情况修改
toolchain_path="/home/book/100ask_imx6ull-sdk/ToolChain/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin"

function cue() {
    echo
    echo "Usage:"
    echo "  ./build.sh -linux"
    echo "  ./build.sh -t113 "
    echo "  ./build.sh -imx6ull "  # 【修改点 2：添加帮助提示】
    echo "  ./build.sh -clean  "
    echo
}

while test $# -gt 0
do
  case "$1" in
  -linux)
    platform="linux"
    ;; 
  -t113)
    platform="t113"
    ;; 
  -imx6ull)                    # 【修改点 3：添加参数解析】
    platform="imx6ull"
    ;;
  -clean)
    rm ./build/ -rf
    echo "clean project success"
    exit 0
    ;;
  -h)
    cue
    exit 0
    ;;
  *)
  esac
  shift
done

if [ ! -d ./build ]; then
    mkdir build
fi

if [ -z ${platform} ]; 
then
    cue
    exit 0
fi

if [ ${platform} == "linux" ];
then
    echo "build linux app"
    cd build
    cmake .. -DCMAKE_TOOLCHAIN_FILE=platform/x86linux/linux.cmake -DSIMULATOR_LINUX=${platform}
    make -j16
    exit 0
fi

if [ ${platform} == "t113" ];
then
    export STAGING_DIR="${toolchain_path}":$STAGING_DIR
    echo "build t113 app"
    cd build
    cmake .. -DCMAKE_TOOLCHAIN_FILE=platform/t113/t113.cmake -DTOOLCHAIN_PATH="${toolchain_path}"
    make -j16
    exit 0
fi

# =========================================================
# 【修改点 4：新增 i.MX6ULL 专属编译分支】
# =========================================================
if [ ${platform} == "imx6ull" ];
then
    export STAGING_DIR="${toolchain_path}":$STAGING_DIR
    echo "build imx6ull app"
    cd build
    # 调用我们接下来要创建的 imx6ull.cmake 工具链文件
    cmake .. -DCMAKE_TOOLCHAIN_FILE=platform/imx6ull/imx6ull.cmake -DTOOLCHAIN_PATH="${toolchain_path}"
    make -j16
    exit 0
fi