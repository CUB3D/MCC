cmake_minimum_required(VERSION 3.9)
project(MCC)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(third_party)

add_executable(MCC src/main.cpp src/mccState.cpp src/mccToken.cpp src/mccToken.h src/mccStateMachine.cpp src/mccStateMachine.h src/mccRegister.cpp src/mccRegister.h src/mccVectorStream.cpp src/mccVectorStream.h src/mccTokenStream.cpp src/mccTokenStream.h)
target_link_libraries(MCC nlohmann_json::nlohmann_json cxxopts stdc++fs)