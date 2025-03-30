cmake_minimum_required(VERSION 3.30)
project(MapSimulation)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_GENERATOR_PLATFORM Win32)

find_package(OpenCV REQUIRED PATHS libs/opencv/ NO_DEFAULT_PATH)

find_package(OpenGL REQUIRED)

add_executable(MapSimulation WIN32)

file(GLOB_RECURSE Sources src/*.cpp)

file(GLOB_RECURSE Libs libs/*.c libs/*.cpp)

target_sources(MapSimulation PRIVATE ${Sources})

target_sources(MapSimulation PRIVATE ${Libs})

target_link_libraries(MapSimulation PRIVATE comctl32)

target_link_libraries(MapSimulation PRIVATE Gdi32.Lib)

target_link_libraries(MapSimulation PRIVATE ${OPENGL_LIBRARIES})

target_link_libraries(MapSimulation PUBLIC ${OpenCV_LIBS})

target_include_directories(MapSimulation PUBLIC ${OpenCV_INCLUDE_DIRS})

target_include_directories(MapSimulation PUBLIC libs/glad/include)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_TYPE "Debug")
else()
    set(BUILD_TYPE "Release")
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/small.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_TYPE}/assets)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/provinces.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_TYPE}/assets)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/provinces1.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_TYPE}/assets)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/libs/opencv/x64/vc16/bin/opencv_world4100.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_TYPE})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/libs/opencv/x64/vc16/bin/opencv_world4100d.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_TYPE})
