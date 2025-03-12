cmake_minimum_required(VERSION 3.15)
project(wyre)

# C++ standard version
set(CMAKE_CXX_STANDARD 20 CACHE STRING "" FORCE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Library target
add_library(wyre STATIC)
target_compile_definitions(wyre PRIVATE WYRE_RUN=0)
set_target_properties(wyre PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME})

# Add compiler (debug & release) definitions
target_compile_definitions(wyre PRIVATE $<$<CONFIG:Debug>:DEBUG=1>)
target_compile_definitions(wyre PRIVATE $<$<CONFIG:Release>:NDEBUG=1>)

# Include directories & pre-compiled header
target_include_directories(wyre PUBLIC "src/")
target_include_directories(wyre PRIVATE "src/wyre/platform/")
# target_precompile_headers(wyre PRIVATE "src/pch.h")

# Glob all source files
file(GLOB_RECURSE src "src/*.cpp")
target_sources(wyre PRIVATE ${src})

# External dependencies
add_subdirectory("extern")

# Don't add example projects if Wyre is used with "add_subdirectory"
if(${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
    # Example projects
    add_subdirectory("examples")
endif()

# Engine assets directory
add_subdirectory("assets")
