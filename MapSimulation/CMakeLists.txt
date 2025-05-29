cmake_minimum_required(VERSION 3.27)
project(MapSimulation)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MapSimulation)

set_target_properties(MapSimulation PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
)

file(GLOB_RECURSE Sources src/*.cpp)

#file(GLOB_RECURSE Libs libs/*.c libs/*.cpp)

target_sources(MapSimulation PRIVATE ${Sources})

#target_sources(MapSimulation PRIVATE ${Libs})

target_include_directories(MapSimulation PRIVATE libs/stb_image)

include(FetchContent)

FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.x   # Change this to the desired version
)

FetchContent_MakeAvailable(sfml)

target_link_libraries(MapSimulation PRIVATE SFML::Graphics SFML::Audio SFML::Network SFML::System SFML::Window)

#set(SFML_DIR libs/SFML-3.0.0/lib/cmake/SFML)
#find_package(SFML 3 COMPONENTS Audio Graphics Network Main System Window REQUIRED)
#if (SFML_FOUND)
#    target_include_directories(MapSimulation PRIVATE libs/SFML-3.0.0/include)
#    target_link_libraries(MapSimulation PRIVATE SFML::Graphics SFML::Audio SFML::Network SFML::System SFML::Window)
#endif ()

#find_package(OpenCV REQUIRED imgproc PATHS libs/opencv/build NO_DEFAULT_PATH)
#if (OpenCV_FOUND)
#    target_include_directories(MapSimulation PRIVATE ${OpenCV_INCLUDE_DIRS})
#    target_link_libraries(MapSimulation PRIVATE ${OpenCV_LIBS})
#endif ()

file(GLOB_RECURSE Images assets/*.png)

file(COPY ${Images} DESTINATION ${CMAKE_BINARY_DIR}/bin/assets)

#file(GLOB_RECURSE OpenCV_DLLS ${OpenCV_LIB_PATH}/../bin/*.dll)
#
#file(COPY ${OpenCV_DLLS} DESTINATION ${CMAKE_BINARY_DIR}/bin)

#file(GLOB_RECURSE SFML_DLLS libs/SFML-3.0.0/bin/*.dll)
#
#file(COPY ${SFML_DLLS} DESTINATION ${CMAKE_BINARY_DIR}/bin)
