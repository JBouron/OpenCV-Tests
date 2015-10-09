cmake_minimum_required(VERSION 3.3)
project(OpenCVTestProject)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(OpenCV_FOUND TRUE)
find_package( OpenCV REQUIRED )
set(SOURCE_FILES main.cpp)
add_executable(OpenCVTestProject ${SOURCE_FILES})
target_link_libraries(OpenCVTestProject ${OpenCV_LIBS})
