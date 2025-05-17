# Zenoh

## Demo

Source code located in `main.c`. This is a simple C struct publisher with Zenoh Pico library. And there is an example of struct receiving with Zenoh Python on PC in `sub.py` file.

Also for communication you need to run Zenoh router on PC (or some another device in your network). Simplest way is do use official docker image:

```bash
docker run --init --net host eclipse/zenoh:main
```

Then you can run built C pub on LicheeRV Nano and `sub.py` on PC.


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
