cmake_minimum_required(VERSION 3.5)

project(barcode_det)
set(CMAKE_CXX_STANDARD 11)

SET(CMAKE_C_COMPILER "$ENV{COMPILER}/riscv64-unknown-linux-musl-gcc")
SET(CMAKE_CXX_COMPILER "$ENV{COMPILER}/riscv64-unknown-linux-musl-g++")
SET(CMAKE_C_LINK_EXECUTABLE "$ENV{COMPILER}/riscv64-unknown-linux-musl-ld")
set(CMAKE_SYSTEM_PROCESSOR arm)

set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/opencv-mobile-4.10.0-licheerv-nano/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(barcode_det main.cpp MJPEGWriter.cpp)

target_link_libraries(barcode_det ${OpenCV_LIBS})
