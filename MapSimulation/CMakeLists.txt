cmake_minimum_required(VERSION 3.30)
project(MapSimulation)

set(CMAKE_CXX_STANDARD 23)

add_executable(MapSimulation)

file(GLOB_RECURSE Sources src/*.cpp)

file(GLOB_RECURSE Libs libs/*.c libs/*.cpp)

target_sources(MapSimulation PRIVATE ${Sources})

target_sources(MapSimulation PRIVATE ${Libs})

set(SFML_DIR libs/SFML-3.0.0/lib/cmake/SFML)
find_package(SFML 3 COMPONENTS Audio Graphics Network Main System Window REQUIRED)
if(SFML_FOUND)
    target_include_directories(MapSimulation PRIVATE libs/SFML-3.0.0/include)
    target_link_libraries(MapSimulation PRIVATE SFML::Graphics SFML::Audio SFML::Network SFML::System SFML::Window)
endif()

find_package(OpenGL REQUIRED)
target_link_libraries(MapSimulation PRIVATE ${OPENGL_LIBRARIES})
target_include_directories(MapSimulation PRIVATE libs/glad/include)

find_package(OpenCV REQUIRED PATHS libs/opencv/ NO_DEFAULT_PATH)
target_link_libraries(MapSimulation PRIVATE ${OpenCV_LIBS})
target_include_directories(MapSimulation PRIVATE ${OpenCV_INCLUDE_DIRS})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_TYPE "Debug")
else()
    set(BUILD_TYPE "Release")
endif()

file(GLOB_RECURSE Images assets/*.png)

file(COPY ${Images} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_TYPE}/assets)

file(GLOB_RECURSE OpenCV_DLLS libs/opencv/x64/vc16/bin/*.dll)

file(COPY ${OpenCV_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_TYPE})

file(GLOB_RECURSE SFML_DLLS libs/SFML-3.0.0/bin/*.dll)

file(COPY ${SFML_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_TYPE})
