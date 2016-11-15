cmake_minimum_required(VERSION 2.6)
Project(get3dpointcloud)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV REQUIRED)

add_executable(get3dpointcloud main.cpp)
target_link_libraries(get3dpointcloud ${OpenCV_LIBS})