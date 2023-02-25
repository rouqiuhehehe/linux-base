cmake_minimum_required(VERSION 3.22)
project(linux-base)

add_subdirectory(mysql-connection-poll)

add_executable(address-book address-book.c)
add_executable(lock lock.c)
add_executable(thread-poll thread-poll.c)

add_executable(mysql mysql.c)
target_include_directories(mysql PUBLIC /usr/include/mysql)
file(GLOB allCopyFile "assets/*")
file(COPY ${allCopyFile} DESTINATION "${CMAKE_BINARY_DIR}/assets")
target_link_libraries(mysql mysqlclient)