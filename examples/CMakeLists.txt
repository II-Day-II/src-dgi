# Function for adding a new example
function(add_example EXAMPLE_NAME)
    set(EXAMPLE_DIR ./${EXAMPLE_NAME})
    set(EXE_NAME wyre-example-${EXAMPLE_NAME})

    # Executable target
    add_executable(${EXE_NAME} "${EXAMPLE_DIR}/main.cpp")
    set_target_properties(${EXE_NAME} PROPERTIES OUTPUT_NAME ${EXE_NAME})
    
    # working directory
    set_property(TARGET ${EXE_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

    # Add compiler (debug & release) definitions
    target_compile_definitions(${EXE_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG=1>)
    target_compile_definitions(${EXE_NAME} PRIVATE $<$<CONFIG:Release>:NDEBUG=1>)

    # Include directories & pre-compiled header
    target_include_directories(${EXE_NAME} PRIVATE "${EXAMPLE_DIR}/")
    target_precompile_headers(${EXE_NAME} PRIVATE "${EXAMPLE_DIR}/pch.h")

    # Link to the Wyre engine library
    target_link_libraries(${EXE_NAME} PRIVATE wyre)
    target_include_directories(${EXE_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src/")
endfunction()

# Add example projects
add_example("basic")
