cmake_minimum_required(VERSION 3.27)
project(png_color_uniformizer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)

# Targets
add_executable(pcu main.c src/transform.c src/display_image.c src/km.c)
add_executable(manual manual.cpp src/display_image.c)
add_executable(pixel src/pixel.c src/display_image.c)

# Libraries
find_package(ZLIB REQUIRED)
find_package(SDL2 REQUIRED)
find_package(PNG REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${PNG_INCLUDE_DIRS})
include_directories(include)
target_link_libraries(pcu m ${ZLIB_LIBRARIES} SDL2::SDL2 ${PNG_LIBRARIES})
target_link_libraries(manual m ${ZLIB_LIBRARIES} SDL2::SDL2)
target_link_libraries(pixel m ${ZLIB_LIBRARIES} ${PNG_LIBRARIES})
