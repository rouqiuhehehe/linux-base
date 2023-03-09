set(CMAKE_CXX_STANDARD 11)
set(TARGET_NAME "bloom-filter")

aux_source_directory(. SOURCE_SRC)
add_executable(${TARGET_NAME} ${SOURCE_SRC})