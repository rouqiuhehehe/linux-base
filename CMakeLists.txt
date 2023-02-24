cmake_minimum_required(VERSION 3.22)
project(linux-base)

add_executable(address-book address-book.c)
add_executable(lock lock.c)
add_executable(thread-poll thread-poll.c)


include_directories(/usr/include/mysql)
add_executable(mysql mysql.c)
target_link_libraries(mysql mysqlclient)