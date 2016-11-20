cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(getStripes)

find_package(OpenCV REQUIRED)

add_executable (getStripes getStripes.cpp)
target_link_libraries (getStripes ${OpenCV_LIBS})