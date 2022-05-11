cmake_minimum_required(VERSION 3.8.0)

project(IG_tools)

add_executable(
    igtool
    igtool/igtool.cpp
)

add_executable(
    igscript
    igscript/igscript.cpp
)

add_executable(
    igscriptD
    igscript/igscriptD.cpp
)
