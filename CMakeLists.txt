cmake_minimum_required(VERSION 3.22)
project(linux-base)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(mysql-connection-poll)

file(GLOB SOURCE_FILE "*.c" "*.cpp")

foreach (FILE ${SOURCE_FILE})
    string(REGEX REPLACE ".*/(.*).([^.]+)" "\\1" TARGET_NAME ${FILE})
    string(REGEX REPLACE ".*/(.*)" "\\1" SOURCE_FILE_NAME ${FILE})

    message("-----------${TARGET_NAME} ${SOURCE_FILE_NAME}----------")
    add_executable(${TARGET_NAME} ${SOURCE_FILE_NAME})
endforeach ()

target_include_directories(mysql PUBLIC /usr/include/mysql)
target_link_libraries(mysql mysqlclient)

file(GLOB allCopyFile "assets/*")
file(COPY ${allCopyFile} DESTINATION "${CMAKE_BINARY_DIR}/assets")