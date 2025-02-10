cmake_minimum_required(VERSION 3.29)
project(MapSimulation)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_GENERATOR_PLATFORM Win32)

find_package(OpenCV REQUIRED PATHS libs/opencv/ NO_DEFAULT_PATH)

add_executable(MapSimulation WIN32 src/main.cpp src/Base/Province.cpp src/Warfare/Army.cpp libs/glad/src/glad.c)

target_link_libraries(MapSimulation PUBLIC "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/um/x64/Gdi32.Lib")

target_link_libraries(MapSimulation PUBLIC "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/um/x64/OpenGl32.Lib")

target_link_libraries(MapSimulation PUBLIC ${OpenCV_LIBS})

target_include_directories(MapSimulation PUBLIC ${OpenCV_INCLUDE_DIRS})

target_include_directories(MapSimulation PUBLIC libs/glad/include)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/small.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/assets)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/provinces.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/assets)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/provinces1.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/assets)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/libs/opencv/x64/vc16/bin/opencv_world4100.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/libs/opencv/x64/vc16/bin/opencv_world4100d.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
