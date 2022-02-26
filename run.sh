#!/bin/sh
cmake -B build/ -DCMAKE_BUILD_TYPE=DEBUG && cmake --build build/ && ./build/bin/opengl
