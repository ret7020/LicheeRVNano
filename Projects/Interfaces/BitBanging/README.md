# Bit Banging for I2C and SPI

You can emulate I2C-5 and SPI-4 on CPU. So, load kernel modules:

```bash
# I2C
insmod /mnt/system/ko/i2c-algo-bit.ko
insmod /mnt/system/ko/i2c-gpio.ko

# SPI
insmod /mnt/system/ko/spi-bitbang.ko
insmod /mnt/system/ko/spi-gpio.ko
```

Next, check that modules loaded correctly:

```bash
lsmod
```

And then you can use interfaces from userspace:

```bash
# I2C
/dev/i2c-5

# SPI
/dev/spidev4.0
```
