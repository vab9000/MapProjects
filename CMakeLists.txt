cmake_minimum_required(VERSION 3.29)
project(MapSimulation)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_GENERATOR_PLATFORM Win32)

find_package(OpenCV REQUIRED PATHS libs/opencv/ NO_DEFAULT_PATH)

add_executable(MapSimulation WIN32 src/main.cpp src/Base/Province.cpp src/Warfare/Army.cpp)

target_link_libraries(MapSimulation PUBLIC ${OpenCV_LIBS})

target_include_directories(MapSimulation PUBLIC ${OpenCV_INCLUDE_DIRS})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/provinces.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/assets)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/provinces1.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/assets)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/libs/opencv/x64/vc16/bin/opencv_world4100.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/libs/opencv/x64/vc16/bin/opencv_world4100d.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
