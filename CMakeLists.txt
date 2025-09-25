cmake_minimum_required(VERSION 3.10)
project(omen-rgb-cli)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(omen-rgb-cli src/main.cpp)

install(TARGETS omen-rgb-cli
    RUNTIME DESTINATION bin
)
