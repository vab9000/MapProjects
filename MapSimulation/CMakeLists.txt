include(FetchContent)

set(SFML_VERSION 3.0.0)
set(IMGUI_VERSION 1.91.1)
set(IMGUI_SFML_VERSION 3.0)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY "https://github.com/SFML/SFML.git"
        GIT_TAG "${SFML_VERSION}"
)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
        GIT_TAG "v${IMGUI_VERSION}"
)

FetchContent_Declare(
        imgui-sfml
        GIT_REPOSITORY "https://github.com/SFML/imgui-sfml.git"
        GIT_TAG "v${IMGUI_SFML_VERSION}"
)

FetchContent_Declare(
        Boost
        URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
        URL_MD5 893b5203b862eb9bbd08553e24ff146a
        DOWNLOAD_EXTRACT_TIMESTAMP ON
)

set(BOOST_INCLUDE_LIBRARIES unordered)
FetchContent_MakeAvailable(Boost)

option(SFML_BUILD_AUDIO "Build audio" OFF)
option(SFML_BUILD_NETWORK "Build network" OFF)
FetchContent_MakeAvailable(sfml)

FetchContent_MakeAvailable(imgui)

set(IMGUI_DIR ${imgui_SOURCE_DIR})
option(IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF)

FetchContent_MakeAvailable(imgui-sfml)

cmake_minimum_required(VERSION 3.27)
project(MapSimulation)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MapSimulation)

target_include_directories(MapSimulation PRIVATE libs/stb_image)

set_target_properties(MapSimulation PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/bin
)

file(GLOB_RECURSE Sources src/*.cpp)

target_sources(MapSimulation PRIVATE ${Sources})

target_link_libraries(MapSimulation PRIVATE SFML::Graphics SFML::System SFML::Window)

target_link_libraries(MapSimulation PUBLIC ImGui-SFML::ImGui-SFML)

target_link_libraries(MapSimulation PUBLIC Boost::unordered)

file(GLOB_RECURSE Images assets/*)
foreach(image ${Images})
    get_filename_component(filename ${image} NAME)
    configure_file(${image} ${CMAKE_BINARY_DIR}/bin/assets/${filename} COPYONLY)
endforeach()

file(GLOB_RECURSE Shaders shaders/*)
foreach(shader ${Shaders})
    get_filename_component(filename ${shader} NAME)
    configure_file(${shader} ${CMAKE_BINARY_DIR}/bin/shaders/${filename} COPYONLY)
endforeach()
