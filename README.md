# LicheeRV Nano Examples
![IMG_20241110_214338](https://github.com/user-attachments/assets/bbc0e666-5dd2-4052-b9cc-615e3a9bda76)


## Repo structure

- Interfaces
    - [UART](Projects/Interfaces/UART)
    
- NN
    - [YoloV8](Projects/Yolov8)
    - [YoloV8_Fast_Test](Projects/Yolov8)

## Pin map
You can enable some pins "in hot" by using devmem (bin in [Tools](Tools)) or changing U-Boot init source in `build/boards/sg200x/sg2002_licheervnano_sd/u-boot/cvi_board_init.c`. Pin map you can find in [GPIO README](Projects/GPIO/README.md).