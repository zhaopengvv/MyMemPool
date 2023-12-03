cmake_minimum_required(VERSION 3.14.1)
project (my_mem_pool)

# 添加头文件搜索路径
include_directories("${PROJECT_SOURCE_DIR}/mempool")
# 添加子文件
add_subdirectory(mempool)
OPTION(TEST "option for test module" OFF)
if (TEST)
    add_subdirectory(test)
endif()

# 将当前目录所有源文件名称保存到 $DIR_SRCS
aux_source_directory(. DIR_SRCS)

# 指定生成目标
ADD_EXECUTABLE(main ${DIR_SRCS})

# 添加链接库
TARGET_LINK_LIBRARIES(main mempool)
