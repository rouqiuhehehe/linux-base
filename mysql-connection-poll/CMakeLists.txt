set(CMAKE_CXX_STANDARD 11)
set(TARGET_NAME "test-connection-poll")
#此处打开为测试连接池
#关闭则编译成静态库
option(TEST "test-connection-poll" ON)

aux_source_directory(. SOURCE_SRC)
if (TEST)
    add_definitions(-DTEST)
    add_executable(${TARGET_NAME} ${SOURCE_SRC})
else ()
    add_library(${TARGET_NAME} STATIC ${SOURCE_SRC})
endif ()
target_include_directories(${TARGET_NAME} PUBLIC /usr/include/mysql)
target_link_libraries(${TARGET_NAME} mysqlclient pthread)