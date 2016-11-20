cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(morphological)

find_package(OpenCV REQUIRED)

add_executable (morphological morphological.cpp)
target_link_libraries (morphological ${OpenCV_LIBS})