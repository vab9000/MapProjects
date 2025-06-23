cmake_minimum_required(VERSION 3.30)
project(ProvinceGenerator)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE COMMON_SOURCE_FILES src/common/*.cpp)

file(GLOB_RECURSE PROVINCE_GENERATOR_SOURCE_FILES src/province_generator/*.cpp)

add_executable(ProvinceGenerator ${PROVINCE_GENERATOR_SOURCE_FILES} ${COMMON_SOURCE_FILES})

file (GLOB_RECURSE PROVINCE_FILES_GENERATOR_SOURCE_FILES src/province_files_generator/*.cpp)

add_executable(ProvinceFilesGenerator ${PROVINCE_FILES_GENERATOR_SOURCE_FILES} ${COMMON_SOURCE_FILES})

target_include_directories(ProvinceGenerator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/stb)

target_include_directories(ProvinceFilesGenerator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/stb)

file(GLOB_RECURSE IMAGES "images/*")

file(COPY ${IMAGES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/images)
