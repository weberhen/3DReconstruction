cmake_minimum_required(VERSION 2.6)
Project(3DReconstruction)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV REQUIRED)
add_executable(3DReconstruction 3DReconstruction.cpp)
target_link_libraries(3DReconstruction ${OpenCV_LIBS})