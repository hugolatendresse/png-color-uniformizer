cmake_minimum_required(VERSION 3.27)
project(png_color_uniformizer)

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_C_STANDARD 99)

# Targets
add_executable(manual manual.cpp)
add_executable(pixel pixel.c)
add_executable(pngtopng pngtopng.c)
add_executable(pngtopngcpp pngtopng.cpp)
target_compile_options(pngtopngcpp PRIVATE -fpermissive)

# Libraries
find_package(ZLIB REQUIRED)
find_package(SDL2 REQUIRED)
find_package(PNG REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${PNG_INCLUDE_DIRS})
target_link_libraries(manual ${ZLIB_LIBRARIES} SDL2::SDL2)
target_link_libraries(pixel ${ZLIB_LIBRARIES} ${PNG_LIBRARIES})
target_link_libraries(pngtopng ${ZLIB_LIBRARIES} ${PNG_LIBRARIES})
target_link_libraries(pngtopngcpp ${ZLIB_LIBRARIES} ${PNG_LIBRARIES})
