cmake_minimum_required(VERSION 3.25)

project(
    amazing_std
    LANGUAGES CXX
    DESCRIPTION "a simple std standard library implementation"
)

option(BUILD_SHARED_LIBS "build shared library" OFF)
option(BUILD_TEST "build test example" ON)

file(GLOB_RECURSE HEADER_FILES include/*.h)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_library(
    ${PROJECT_NAME}
)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(
    ${PROJECT_NAME}
    PUBLIC
    cxx_std_23
)

if (MSVC)
    target_compile_options(
        ${PROJECT_NAME}
        PUBLIC
        # make __VA_OPT__ valid in msvc
        /Zc:preprocessor
        # disable alternative tokens
        /Za
        # utf-8 source and execution character sets
        /utf-8
    )
endif ()

if (BUILD_TEST)
add_executable(
    ${PROJECT_NAME}_test
    test/main.cpp
)

target_link_libraries(
    ${PROJECT_NAME}_test
    PRIVATE
    ${PROJECT_NAME}
)
endif ()