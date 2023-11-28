cmake_minimum_required(VERSION 3.14.1)
project (my_mem_pool)

add_subdirectory(mempool)
OPTION(TEST "option for test module" OFF)
if (TEST)
    add_subdirectory(test)
endif()

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/mempool)
TARGET_LINK_LIBRARIES(main libmempool.so)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/bin) 

ADD_EXECUTABLE(main main.c)
