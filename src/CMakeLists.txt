cmake_minimum_required(VERSION 3.18)

project(state_machine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(SOURCES 
    ./main.cpp
    ./db/db.cpp
    ./tool/tool.cpp
    ./sock_data/sock_data.cpp
    ./3rdparty/CLI11.hpp
)

add_executable(state_machine ${SOURCES})

target_link_libraries(state_machine
    mysqlclient
)