cmake_minimum_required(VERSION 2.6)
Project(calibrateGrayScale)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV REQUIRED)

add_executable(calibrateGrayScale main.cpp)
target_link_libraries(calibrateGrayScale ${OpenCV_LIBS})