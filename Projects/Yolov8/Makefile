

build:
	mkdir -p ./bin
	riscv64-unknown-linux-musl-g++ -march=rv64imafd -O3 -DNDEBUG -D_MIDDLEWARE_V2_ -DC906 -DUSE_TPU_IVE -std=gnu11 -Wno-pointer-to-int-cast -fsigned-char -Werror=all -Wno-format-truncation -fdiagnostics-color=always -s -I$(SDK_PATH)/cvitek_tdl_sdk/include -I$(SDK_PATH)/cvitek_tdl_sdk/include/cvi_tdl -I$(SDK_PATH)/cvitek_tdl_sdk/include/cvi_tdl_app -I$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/include -I$(SDK_PATH)/cvitek_tdl_sdk/sample/utils -I$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/rtsp/include/cvi_rtsp -I$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/ive/include -I$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/opencv/include -I$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/stb/include -I$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/middleware/v2/sample/common -I$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/middleware/v2/include/isp/cv180x -I$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/middleware/v2/include/panel -I$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/middleware/v2/include -I -I$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/middleware/v2/include -I$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/middleware/v2/include/linux -I/include/stb -o ./bin/sample_yolov8.o -c main.c
	riscv64-unknown-linux-musl-g++ -mcpu=c906fdv -L$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/middleware/v2/lib -L$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/middleware/v2/lib/3rd -lini -lsns_full -lsample -lisp -lvdec -lvenc -lawb -lae -laf -lcvi_bin -lcvi_bin_isp -lmisc -lisp_algo -lsys -lvpu -L$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/opencv/lib -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -L$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/tpu/lib -lcnpy -lcvikernel -lcvimath -lcviruntime -lz -lm -L$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/ive/lib -lcvi_ive_tpu -L$(SDK_PATH)/cvitek_tdl_sdk/lib -lcvi_tdl -L$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/lib -lpthread -latomic -o ./bin/sample_yolov8 ./bin/sample_yolov8.o
	rm ./bin/sample_yolov8.o

clean:
	rm -r bin
