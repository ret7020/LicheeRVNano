cmake_minimum_required(VERSION 3.10)
project(ros2_bridge_test)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_COMPILER "$ENV{COMPILER}/riscv64-unknown-linux-musl-g++")
SET(CMAKE_CXX_COMPILER "$ENV{COMPILER}/riscv64-unknown-linux-musl-g++")
SET(CMAKE_C_LINK_EXECUTABLE "$ENV{COMPILER}/riscv64-unknown-linux-musl-ld")

set(CMAKE_CXX_FLAGS "-march=rv64imafd -O3 -DNDEBUG -D_MIDDLEWARE_V2_ -DC906 -DUSE_TPU_IVE -fsigned-char -Wno-format-truncation -fdiagnostics-color=always -s -Wno-address-of-packed-member")


set(SOURCE_FILES main.cpp)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

add_executable(ros2_bridge_test ${SOURCE_FILES})

target_link_libraries(ros2_bridge_test
    -mcpu=c906fdv
)

