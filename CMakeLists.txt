cmake_minimum_required(VERSION 3.29)
project(MapSimulation)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_GENERATOR_PLATFORM Win32)

add_executable(MapSimulation WIN32 src/main.cpp src/Base/Province.cpp)

include_directories(libs/stb/include)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/provinces.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/assets)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/small.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/assets)
