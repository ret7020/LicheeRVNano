cmake_minimum_required(VERSION 3.13)
project(zenoh_pub C)

# Set cross-compiler
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER $ENV{COMPILER}/riscv64-unknown-linux-musl-gcc)

# Set C standard and compile options
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(
    -DZENOH_COMPILER_GCC
    -DZENOH_C_STANDARD=11
    -DZENOH_DEBUG=3
    -DZENOH_LINUX
    -DZ_BUILD_LOG=0
    -O3
    -DNDEBUG
    -std=gnu11
    -pipe
)

# Include and link directories
include_directories(./zenoh/include/)
link_directories(./zenoh/)

# Add executable
add_executable(zenoh_pub main.c)

# Link libraries
target_link_libraries(zenoh_pub zenohpico)
