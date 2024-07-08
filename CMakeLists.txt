cmake_minimum_required(VERSION 3.27)
project(png_color_uniformizer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)

# Target for main.cpp
add_executable(main main.cpp)

# Target for pngpixel.c
add_executable(pngpixel pngpixel.c)

# Target for pngtopng.c
add_executable(pngtopng pngtopng.c)


# Need zlib library
find_package(ZLIB REQUIRED)
find_package(SDL2 REQUIRED)
find_package(PNG REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${PNG_INCLUDE_DIRS})
target_link_libraries(main ${ZLIB_LIBRARIES} SDL2::SDL2)
target_link_libraries(pngpixel ${ZLIB_LIBRARIES} ${PNG_LIBRARIES})
target_link_libraries(pngtopng ${ZLIB_LIBRARIES})
message(STATUS "ZLIB_INCLUDE_DIRS: ${ZLIB_INCLUDE_DIRS}")
message(STATUS "ZLIB_LIBRARIES: ${ZLIB_LIBRARIES}")