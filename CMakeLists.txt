cmake_minimum_required(VERSION 3.0)

project(loadBalancer)

set(CMAKE_CXX_STANDARD 98)

add_executable(loadBalancer main.cpp LoadBalancer.cpp LoadBalancer.h)

target_link_libraries(loadBalancer pthread)
