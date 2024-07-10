cmake_minimum_required(VERSION 3.0)
project(png_color_uniformizer)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)

# Main target
add_executable(pcu main.c src/transform.c src/display_image.c src/km.c)
add_executable(dbg-pcu main.c src/transform.c src/display_image.c src/km.c)
target_compile_definitions(dbg-pcu PRIVATE DEBUG)

# Other targets
add_executable(patch src/patch.c src/transform.c src/display_image.c src/km.c)
add_executable(manual src/manual.cpp src/display_image.c)
add_executable(pixel src/pixel.c src/display_image.c)

# Libraries
find_package(ZLIB REQUIRED)
find_package(SDL2 REQUIRED)
find_package(PNG REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${PNG_INCLUDE_DIRS})
include_directories(include)
target_link_libraries(pcu m ${ZLIB_LIBRARIES} SDL2::SDL2 ${PNG_LIBRARIES})
target_link_libraries(dbg-pcu m ${ZLIB_LIBRARIES} SDL2::SDL2 ${PNG_LIBRARIES})
target_link_libraries(patch m ${ZLIB_LIBRARIES} SDL2::SDL2 ${PNG_LIBRARIES})
target_link_libraries(manual m ${ZLIB_LIBRARIES} SDL2::SDL2)
target_link_libraries(pixel m ${ZLIB_LIBRARIES} ${PNG_LIBRARIES})
