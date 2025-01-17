# YoloV8 tests

Required dynamic libs (.so) can be downloaded [here](https://drive.google.com/file/d/1ZM2aeyJBJViDAIc7k13jI9BEQCafPyvE/view?usp=sharing). After downloading to board and extracting zip use `board_setup.sh` to set `LD_LIBRARY_PATH`

It is raw tests, makefile is awful for now.

How to compile:
```bash
# export cross compilers (musl stdlib) to current shell
export PATH=$PATH:/home/stephan/Downloads/LicheeRV_CrossCompilers/gcc/riscv64-linux-musl-x86_64/bin
# export SDK into SDK_PATH var
export SDK_PATH=/home/stephan/Downloads/CVI_SDK

make build
```


| Model          | Classes | IMG       | FPS | mAP50 |
| ----------     | ----    | ------    | --- | ----- |
| Yolov8n COCO   |  80     | 640x640   | 17  |       |
| Yolov8n Custom |  1      | 640x640   | 27  |       |
| Yolov8n Custom |  2      | 640x640   | N/A |       |
| Yolov8n COCO   |  80     | 320x320   | N/A |       |
| Yolov8n Custom |  1      | 320x320   | N/A |       |
| Yolov8n Custom |  2      | 320x320   | N/A |       |
