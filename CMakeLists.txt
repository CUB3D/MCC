cmake_minimum_required(VERSION 3.13)
project(MCC)

set(CMAKE_CXX_STANDARD 17)

add_executable(MCC src/main.cpp src/mccState.cpp src/MccToken.cpp src/MccToken.h)
target_link_libraries(MCC stdc++fs)