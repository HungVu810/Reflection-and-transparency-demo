cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
# set gcc output color
set(CMAKE_C_FLAGS "-fdiagnostics-color=always")
set(CMAKE_CXX_FLAGS "-std=c++11 -fdiagnostics-color=always -fsanitize=address")

# binary name
project(opengl)

# include files
include_directories(include)

# output path for binary, lib and archive
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(OPENGL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(OPENGL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

# set(COMPILE_FLAGS "-std=c++11 -Wall -pedantic-errors -Werror -g")

file(GLOB SOURCE_FILES src/*)
# set(SOURCE_FILES
#     src/main.cpp
#     src/glad.c
#     src/runtime_except.cpp
#     src/gl_object.cpp
#     src/gl_context.cpp
#     src/gl_vao.cpp
#     src/gl_vbo.cpp
#     src/gl_shader.cpp
#     src/gl_program.cpp
# )
set(DEPENDANT_LIBS
    :libglfw3.a
    GL
    X11
    pthread
    Xrandr
    Xi
    dl
    assimp
)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
# set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
target_link_libraries(${PROJECT_NAME} ${DEPENDANT_LIBS})
# add_library(depend_libs STATIC l:libglfw3 GL X11 pthread Xrandr Xi dl)
