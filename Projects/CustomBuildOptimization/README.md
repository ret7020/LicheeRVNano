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