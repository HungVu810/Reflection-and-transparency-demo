cmake_minimum_required(VERSION 3.10)

# IF THE EXECUTABLE IS NOT UP-TO-DATE, DELETE ./BUILD AND REBUILD (LIKE CMAKE CLEAN)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
# set compile
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# disable -fsanitize to to work with renderdoc
# # set gcc output color
set(CMAKE_C_FLAGS "-fdiagnostics-color=always")
set(CMAKE_CXX_FLAGS "-std=c++11 -fdiagnostics-color=always")
# set(CMAKE_CXX_FLAGS "-std=c++11 -fdiagnostics-color=always -fsanitize=address")
# set(COMPILE_FLAGS "-std=c++11 -Wall -pedantic-errors -Werror -g")

# binary name
project(opengl)

# -I, used target_include_directories to be more specific (below) to a specified 
# target instead of global
# https://stackoverflow.com/questions/31969547/what-is-the-difference-between-include-directories-and-target-include-directorie
# include_directories(include ../imgui/ ../imgui/backends/)

# output path for binary, lib and archive. DOESN'T WORK
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/archive)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# set(OPENGL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
# set(OPENGL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

# to get rid of [abi::cx11]
# add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)

# explicit listing of the source files are better than globing
# https://stackoverflow.com/questions/1027247/is-it-better-to-specify-source-files-with-glob-or-each-file-individually-in-cmak
# file(GLOB SOURCE_FILES src/*)

# https://stackoverflow.com/questions/70250551/failure-to-compile-imgui-glfw-opengl-on-linux-with-gcc-11-2
# file(GLOB IMGUI_SOURCE_FILES
# 	../imgui/imgui*.cpp
# )

# set(IMGUI_HEADERS_PATH
# 	../imgui/
# 	../imgui/backends/
# )

file(GLOB SOURCE_FILES
	src/*.cpp
	src/*.c
	# src/camera.cpp
	# src/gl_context.cpp
	# src/gl_object.cpp
	# src/gl_program.cpp
	# src/gl_shader.cpp
	# src/gl_texture.cpp
	# src/gl_vao.cpp
	# src/gl_vbo.cpp
	# src/glad.c
	# src/handleInput.cpp
	# src/light_model.cpp
	# src/main.cpp
	# src/model.cpp
	# src/runtime_except.cpp
)

set(DEPENDANT_LIBS
    :libglfw3.a
	:librenderdoc.so
    GL
    X11
    pthread
    Xrandr
    Xi
    dl
    assimp
)

# generate executable target
add_executable(${PROJECT_NAME}
	${SOURCE_FILES}
	# ${IMGUI_SOURCE_FILES}
	# ../imgui/backends/imgui_impl_glfw.cpp
	# ../imgui/backends/imgui_impl_opengl3.cpp
)
# target_... to add important libs/dir/files/flags to the target EXECUTABLE
# -I TO ADD <DIR> INTO HEADER SEARCH LIST OF ${PROJECT_NAME} target, used to 
# find headers not specified in main.cpp.
# OUT-OF-SOURCE BUILD only
target_include_directories(${PROJECT_NAME} PRIVATE
	./include
	# ${IMGUI_HEADERS_PATH}
)
# -L
target_link_libraries(${PROJECT_NAME} ${DEPENDANT_LIBS})
