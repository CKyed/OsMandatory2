cmake_minimum_required(VERSION 3.17)
project(OsMandatory2 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(OsMandatory2
        memorytests.c
        mymem.c
        mymem.h
        testrunner.c
        testrunner.h)
