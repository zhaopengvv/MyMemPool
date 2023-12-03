# 将当前目录所有源文件名称保存到 $SRC_LIST
aux_source_directory (. SRC_LIST)

# 生成动态链接库
add_library (mempool SHARED ${SRC_LIST})

