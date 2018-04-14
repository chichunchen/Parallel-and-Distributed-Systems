cmake_minimum_required(VERSION 3.10)
project(src)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(src
        atomicgraph.h
        blocking_queue.h
        lockfree_queue.h
        serialqueue.h
        simplegraph.h
        sssp_blocking.cpp
#        sssp_serial.cpp
#        test_blocking.cpp
#        test_lockfree_queue.cpp
        Timer.h)
