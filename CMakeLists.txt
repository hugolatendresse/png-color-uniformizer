cmake_minimum_required(VERSION 3.27)
project(png_color_uniformizer)

set(CMAKE_CXX_STANDARD 14)

add_executable(png_color_uniformizer main.cpp)

# Need zlib library
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
target_link_libraries(png_color_uniformizer ${ZLIB_LIBRARIES})
message(STATUS "ZLIB_INCLUDE_DIRS: ${ZLIB_INCLUDE_DIRS}")
message(STATUS "ZLIB_LIBRARIES: ${ZLIB_LIBRARIES}")