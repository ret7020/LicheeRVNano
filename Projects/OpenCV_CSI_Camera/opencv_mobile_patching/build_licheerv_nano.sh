# export cross-compiler:  export RISCV_ROOT_PATH=/tmp/host-tools/gcc/riscv64-linux-musl-x86_64
# patch base source
patch -p1 -i opencv-4.10.0-no-atomic.patch

mkdir -p build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE="./toolchains/riscv64-unknown-linux-musl.toolchain.cmake" -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=Release `cat ../options.txt` -DBUILD_opencv_world=OFF ..
make -j18
make install
