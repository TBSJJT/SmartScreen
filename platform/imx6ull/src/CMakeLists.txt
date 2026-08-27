aux_source_directory(porting SOURCE)
# 【病灶切除 1】：不再编译 g2d 目录下的 sunxifb.c 和 sunxig2d.c
# aux_source_directory(porting/g2d SOURCE) 
aux_source_directory(hal SOURCE)
aux_source_directory(utils SOURCE)

add_library(lvgl_porting ${SOURCE})

target_include_directories(lvgl_porting PUBLIC 
    porting
)

target_include_directories(lvgl_porting PRIVATE 
    hal
    porting
    # 【病灶切除 2】：不再包含 g2d 的头文件目录
    # porting/g2d
    utils
    ${CMAKE_SOURCE_DIR}/lvgl
    ${CMAKE_SOURCE_DIR}/component/hal
)

target_compile_definitions(lvgl_porting PRIVATE 
    -DLV_LVGL_H_INCLUDE_SIMPLE
    -DLV_CONF_INCLUDE_SIMPLE
    # 【病灶切除 3】：所有全志专属的宏定义（USE_SUNXIFB等）已全部删除！
    # 这能保证代码里即使残留了 #ifdef USE_SUNXIFB 也会被编译器完美忽略。
)
