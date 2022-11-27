#!/bin/sh
cmake -G Ninja -B build/ -DCMAKE_BUILD_TYPE=DEBUG && cmake --build build/
