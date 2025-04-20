cmake_minimum_required(VERSION 3.10)
project(ive_test)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_COMPILER "$ENV{COMPILER}/riscv64-unknown-linux-musl-g++")
SET(CMAKE_CXX_COMPILER "$ENV{COMPILER}/riscv64-unknown-linux-musl-g++")
SET(CMAKE_C_LINK_EXECUTABLE "$ENV{COMPILER}/riscv64-unknown-linux-musl-ld")

set(CMAKE_CXX_FLAGS "-march=rv64imafd -O3 -DNDEBUG -D_MIDDLEWARE_V2_ -DC906 -DUSE_TPU_IVE -fsigned-char -Werror=all -Wno-format-truncation -fdiagnostics-color=always -s")

include_directories(
    $ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/middleware/v2/include
    $ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/middleware/v2/include/linux
    $ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/ive/include/ive
)

set(SOURCE_FILES main.cpp)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

add_executable(ive_test ${SOURCE_FILES})

target_link_libraries(ive_test
    -mcpu=c906fdv
    -L$ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/middleware/v2/lib
    -L$ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/ive/lib
    -L$ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/middleware/v2/lib/3rd
    -L$ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/tpu/lib
    -lcnpy -lcvikernel -lcvimath -lcviruntime -lz -lm    
    -lpthread -latomic -lcvi_ive_tpu
)