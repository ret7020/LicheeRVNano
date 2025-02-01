# LicheeRV Nano Examples
![IMG_20241110_214338](https://github.com/user-attachments/assets/bbc0e666-5dd2-4052-b9cc-615e3a9bda76)

## Repo structure

- Interfaces
    - [UART](Projects/Interfaces/UART)
    - [SPI](Projects/Interfaces/SPI)
    - [I2C](Projects/Interfaces/I2C)
    - [GPIO](Projects/GPIO)
    - - [Interrupts handling](Projects/Interrupts)

- CSI Camera
    - [OpenCV + CSI read](Projects/OpenCV_CSI_Camera)
    - [Stream CSI to MJPEG](Projects/MJPEGStream)

- OTG Camera
    - [Read USB camera](Projects/OTGCamera)

- NN
    - [YoloV8](Projects/Yolov8)
    - [YoloV8_Fast_Test](Projects/Yolov8)
    - [Yolo Benchmark](Projects/Yolo_Benchmark)
    - [Yolo Camera](Projects/YoloCamera)
    - [LLama](Projects/LLama)

- Other
    - [FreeRTOS](Projects/FreeRTOS)
    - [Docker](Projects/Docker)
    - [Hello world](Projects/HelloWorld)

## Useful links
- [Power usage tests](https://docs.google.com/document/d/1rqYGW91RLbdtiySxgSRCPxvIl0TrY_t7PZlEEJFMUWQ/edit?usp=sharing)

## Building examples
Examples located in [Projects](Projects/). Each project/dir have a Makefile that have a build directive to compile project. It use `${COMPILER}/riscv64-unknown-linux-musl-g++` as compiler; so assing env variable COMPILER with path to SDK location.

## Pin map
You can enable some pins "in hot" by using devmem (bin in [Tools](Tools)) or changing U-Boot init source in `build/boards/sg200x/sg2002_licheervnano_sd/u-boot/cvi_board_init.c`. Pin map you can find in [GPIO README](Projects/GPIO/README.md).
