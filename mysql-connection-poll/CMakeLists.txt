set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. SOURCE_SRC)
add_executable(test-connection-poll ${SOURCE_SRC})
target_include_directories(test-connection-poll PUBLIC /usr/include/mysql)
target_link_libraries(test-connection-poll mysqlclient)