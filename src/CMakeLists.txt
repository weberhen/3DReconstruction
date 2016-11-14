cmake_minimum_required(VERSION 2.6)
Project(get3dpointcloud)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV REQUIRED)

find_package(PCL REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_executable(get3dpointcloud main.cpp)
target_link_libraries(get3dpointcloud ${PCL_LIBRARIES} ${OpenCV_LIBS})