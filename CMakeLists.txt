cmake_minimum_required(VERSION 3.10)
project(BarAqua)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/main.cpp
    src/user.cpp
    src/userManager.cpp
)

add_executable(RFIDBar ${SOURCES})