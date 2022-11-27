# GLFW, assimp and glm are installed via apt install
# Only glad is done manually

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

# output path for binary, lib and archive
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/archive)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# set(OPENGL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
# set(OPENGL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

# to get rid of [abi::cx11]
# add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)

# explicit listing of the source files are better than globing
# https://stackoverflow.com/questions/1027247/is-it-better-to-specify-source-files-with-glob-or-each-file-individually-in-cmak
# https://stackoverflow.com/questions/70250551/failure-to-compile-imgui-glfw-opengl-on-linux-with-gcc-11-2
# https://stackoverflow.com/questions/13703647/how-to-properly-add-include-directories-with-cmake

set(INCLUDES_DIRECTORY "C:/Users/hungd/Documents/Library/Projects/OpenGL/opengl/third-party/includes")
set(LIBS_DIRECTORY "C:/Users/hungd/Documents/Library/Projects/OpenGL/opengl/third-party/libs")

file(GLOB SOURCE_FILES
	src/*.cpp
	src/*.c
	# ${INCLUDES_DIRECTORY}/imgui/imgui*.cpp
	# ${INCLUDES_DIRECTORY}/imgui/backends/imgui_impl_glfw.cpp
	# ${INCLUDES_DIRECTORY}/imgui/backends/imgui_impl_opengl3.cpp
)

set(HEADERS
	# ${INCLUDES_DIRECTORY}/imgui/imgui.h
	# ${INCLUDES_DIRECTORY}/imgui/backends
	${INCLUDES_DIRECTORY}/assimp
	${INCLUDES_DIRECTORY}/glm
	${INCLUDES_DIRECTORY}/glad
	${INCLUDES_DIRECTORY}/GLFW
	${INCLUDES_DIRECTORY}/KHR
)

set(DEPENDANT_LIBS
	opengl32.lib
	${LIBS_DIRECTORY}/glfw3.lib
	${LIBS_DIRECTORY}/assimp-vc143-mt.lib
)

# NOTE: the dll is in the bin folder of build so we don't have to add another path for PATH env var.

# generate executable target
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADERS})

# target_... to add important libs/dir/files/flags to the target EXECUTABLE
# -I TO ADD <DIR> INTO HEADER SEARCH LIST OF ${PROJECT_NAME} target, used to 
# find headers not specified in main.cpp.
# OUT-OF-SOURCE BUILD only
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES_DIRECTORY})#../imgui/)

# -L
target_link_libraries(${PROJECT_NAME} ${DEPENDANT_LIBS})


