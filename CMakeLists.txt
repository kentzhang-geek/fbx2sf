cmake_minimum_required(VERSION 3.0)
project(rpd_fbx_plugin)
include_directories(fbs_generated)

set(SRC
        src/main.cpp
        )

add_executable(test ${SRC})