cmake_minimum_required(VERSION 3.10)
project(BarAqua)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/main.cpp
    src/user.cpp
    src/userManager.cpp
    src/gui.cpp
)

add_executable(RFIDBar ${SOURCES})
target_include_directories(RFIDBar PRIVATE src)

if(UNIX)
    target_link_libraries(RFIDBar pigpio pthread SDL2 SDL2_ttf)
endif()