# Zenoh

## Demo

Source code located in `main.c`


## Compile so library
In `./zenoh` you can find precompiled `libzenohpico.so` and headers for version `1.3.4`. To compile your own zenoh lib you need to:

* Fetch source code, from GitHub releases or upstream repo
* Prepare toolchain file for target platform (RISCV, musl)
```
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=riscv64.toolchain.cmake
```

riscv64.toolchain.cmake (set env var COMPILER with path to host-tools):

```
set(COMPILER $ENV{COMPILER})

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER $ENV{COMPILER}/riscv64-unknown-linux-musl-gcc)
set(CMAKE_CXX_COMPILER $ENV{COMPILER}/riscv64-unknown-linux-musl-g++)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

```

* Make it:
```
make -j10
```

Finally, so file located in `./build/lib`