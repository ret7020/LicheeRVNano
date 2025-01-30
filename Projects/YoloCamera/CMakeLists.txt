cmake_minimum_required(VERSION 3.10)
project(sample_yolov8)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_COMPILER "$ENV{COMPILER}/riscv64-unknown-linux-musl-g++")
SET(CMAKE_CXX_COMPILER "$ENV{COMPILER}/riscv64-unknown-linux-musl-g++")
SET(CMAKE_C_LINK_EXECUTABLE "$ENV{COMPILER}/riscv64-unknown-linux-musl-ld")

set(CMAKE_CXX_FLAGS "-march=rv64imafd -O3 -DNDEBUG -D_MIDDLEWARE_V2_ -DC906 -DUSE_TPU_IVE -fsigned-char -Werror=all -Wno-format-truncation -fdiagnostics-color=always -s")

set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/opencv-mobile-4.10.0-licheerv-nano/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


include_directories(
    $ENV{SDK_PATH}/cvitek_tdl_sdk/include
    $ENV{SDK_PATH}/cvitek_tdl_sdk/include/cvi_tdl
    $ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/middleware/v2/include
    $ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/middleware/v2/include/linux
    $ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/middleware/v2/include/isp/cv181x
    $ENV{SDK_PATH}/cvitek_tdl_sdk/sample/utils
    $ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/rtsp/include/cvi_rtsp
    # ${OpenCV_INCLUDE_DIRS}
)

set(SOURCE_FILES main.c)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

add_executable(sample_yolov8 ${SOURCE_FILES})

target_link_libraries(sample_yolov8
    -mcpu=c906fdv
    -L$ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/middleware/v2/lib
    -L$ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/middleware/v2/lib/3rd
    -lini -lsns_full -lsample -lisp -lvdec -lvenc -lawb -lae -laf -lcvi_bin -lcvi_bin_isp -lmisc -lisp_algo -lsys -lvpu
    -L$ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/opencv/lib
    -lopencv_core -lopencv_imgproc -lopencv_imgcodecs
    -L$ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/tpu/lib
    -lcnpy -lcvikernel -lcvimath -lcviruntime -lz -lm
    -L$ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/ive/lib
    -lcvi_ive_tpu
    -L$ENV{SDK_PATH}/cvitek_tdl_sdk/lib
    -lcvi_tdl
    -L$ENV{SDK_PATH}/cvitek_tdl_sdk/sample/3rd/lib
    -L$ENV{SDK_PATH}/cvitek_tdl_sdk/sample/utils
    -lpthread -latomic
    ${OpenCV_LIBS}
)

