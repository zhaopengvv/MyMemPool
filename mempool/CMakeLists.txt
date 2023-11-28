aux_source_directory (. SRC_LIST)

# 使用${SRC_LIST}为原材料生成一个叫做 mem_pool_shared 的动态库
add_library (mem_pool_shared SHARED ${SRC_LIST})
# 使用${SRC_LIST}为原材料生成一个叫做 mem_pool_static 的静态库
add_library (mem_pool_static STATIC ${SRC_LIST})

# 将 mem_pool_shared 重新命名为 libmempool.so
set_target_properties (mem_pool_shared PROPERTIES OUTPUT_NAME "mempool")
# 将 mem_pool_static 重新命名为 libmempool.a
set_target_properties (mem_pool_static PROPERTIES OUTPUT_NAME "mempool")

# 设置库文件的默认输出路径
set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/lib)
