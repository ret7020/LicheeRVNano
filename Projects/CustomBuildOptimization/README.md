# Buildroot optimization

The official image (version 20250114) weighs about 1.6 Gb, the main part of this weight is overhead from unnecessary packages in the buildroot.

The optimized image contains support for working with the network (Wi-Fi and Ethernet, but utilities for Bluetooth are removed), in the short one, utilities for the network are removed.

You can see detailed changes in packages using the diff command between my configs and configs from the official sipeed repository. But some packages, that removed:
* Python + its packages
* GDB
* curl
* Test tools (ramtest, stress-ng and others)
* Bluez
* OpenCV
* FFmpeg


## Change available RAM in Linux

By default, 105 Mb used for Multimedia Buffer (CSI cameras, DSI screens). So, if you don't use some of them you can increase this buffer memory size. So, you need to edit file: `./build/boards/sg200x/sg2002_licheervnano_sd/memmap.py`.

Set:

```python
ION_SIZE = 20 * SIZE_1M # replace 20 Mb with desired size (default 105)
```

And then rebuild kernel. While kernel build it will generate file: `./build/output/sg2002_licheervnano_sd/cvi_board_memmap.h` with correct memory mapping.