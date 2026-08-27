set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# ====================================================================
# 【修改点 1：替换为百问网 i.MX6ULL 的编译器前缀】
# ====================================================================
set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/arm-buildroot-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/arm-buildroot-linux-gnueabihf-g++)

# ====================================================================
# 【预警点：私有库路径】
# 如果编译时报 "skipping incompatible xxx.so" 的错误，说明这个目录下的库是全志独有的
# ====================================================================
add_link_options(-L${CMAKE_CURRENT_LIST_DIR}/lib)
add_link_options(-L${CMAKE_SOURCE_DIR}/wifi/libs/)
add_link_options(-L${CMAKE_SOURCE_DIR}/net/libs/)

# 【预警点：-luapi】
# 里面的 -luapi 看起来像是全志特定的 API 库。如果报错 "cannot find -luapi"，请把 -luapi 删掉！
add_link_options(-lpthread -lfreetype -lrt -ldl -znow -zrelro -luapi -lm -lz -lbz2 -O0 -rdynamic -g -funwind-tables -ffunction-sections)
add_link_options(-fPIC -Wl,-gc-sections)

add_compile_options(-I${CMAKE_CURRENT_LIST_DIR}/src/porting)
add_compile_options(-I${CMAKE_SOURCE_DIR})
add_compile_options(-I${CMAKE_SOURCE_DIR}/lvgl/demos)
add_compile_options(-I${CMAKE_CURRENT_LIST_DIR}/include)
add_compile_options(-I${CMAKE_CURRENT_LIST_DIR}/include/freetype)

# ====================================================================
# 【架构参数：完美兼容，保持原样！】
# T113 和 i.MX6ULL 都是 Cortex-A7，都带硬浮点(hard)和 Neon 协处理器
# ====================================================================
add_compile_options(-march=armv7-a -mtune=cortex-a7 -mfpu=neon -mfloat-abi=hard -O0 -ldl -rdynamic -g -funwind-tables -ffunction-sections)