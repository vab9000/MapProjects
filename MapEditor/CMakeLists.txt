cmake_minimum_required(VERSION 4.0)
include(FetchContent)
project(MapEditor)

set(GLFW_VERSION 3.4)
set(IMGUI_VERSION 1.92.5)

FetchContent_Declare(
        GLFW
        GIT_REPOSITORY "https://github.com/glfw/glfw.git"
        GIT_TAG "${GLFW_VERSION}"
)

FetchContent_Declare(
        IMGUI
        GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
        GIT_TAG "v${IMGUI_VERSION}"
)

FetchContent_MakeAvailable(GLFW)
FetchContent_MakeAvailable(IMGUI)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.c*" "${CMAKE_SOURCE_DIR}/src/*.h*")

file(GLOB_RECURSE LIB_SOURCES "${CMAKE_SOURCE_DIR}/libs/*/*.c")

add_executable(MapEditor)

target_sources(MapEditor PRIVATE ${SOURCES} ${LIB_SOURCES})

file(GLOB LIB_INCLUDES LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/libs/*/include/")

target_include_directories(MapEditor PRIVATE ${LIB_INCLUDES})

target_include_directories(MapEditor PRIVATE ${CMAKE_SOURCE_DIR}/utils/)

target_link_libraries(MapEditor PRIVATE glfw)

file(GLOB_RECURSE IMGUI_SOURCES
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui_demo.cpp"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_tables.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
        "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
)

target_sources(MapEditor PRIVATE ${IMGUI_SOURCES})

target_include_directories(MapEditor PRIVATE ${imgui_SOURCE_DIR})

set_target_properties(MapEditor PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/bin
)

file(GLOB_RECURSE SHADERS "${CMAKE_SOURCE_DIR}/shaders/*")

foreach (shader ${SHADERS})
    get_filename_component(shader_name ${shader} NAME)
    configure_file(${shader} ${CMAKE_BINARY_DIR}/bin/shaders/${shader_name} COPYONLY)
endforeach ()

file(GLOB_RECURSE IMAGES "${CMAKE_SOURCE_DIR}/images/*")

foreach (image ${IMAGES})
    get_filename_component(image_name ${image} NAME)
    configure_file(${image} ${CMAKE_BINARY_DIR}/bin/images/${image_name} COPYONLY)
endforeach ()
