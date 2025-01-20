CC = $(COMPILER)/riscv64-unknown-linux-musl-g++
CFLAGS += -march=rv64imafd -O3 -DNDEBUG -D_MIDDLEWARE_V2_ -DC906 -DUSE_TPU_IVE -fsigned-char -Werror=all -Wno-format-truncation -fdiagnostics-color=always -s
CFLAGS += -I$(SDK_PATH)/cvitek_tdl_sdk/include \
		-I$(SDK_PATH)/cvitek_tdl_sdk/include/cvi_tdl \
		-I$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/middleware/v2/include \
		
build:
	mkdir -p ./bin
	$(CC) $(CFLAGS) -o ./bin/sample_yolov8.o -c main.c
	$(CC) -mcpu=c906fdv -L$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/middleware/v2/lib -L$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/middleware/v2/lib/3rd -lini -lsns_full -lsample -lisp -lvdec -lvenc -lawb -lae -laf -lcvi_bin -lcvi_bin_isp -lmisc -lisp_algo -lsys -lvpu -L$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/opencv/lib -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -L$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/tpu/lib -lcnpy -lcvikernel -lcvimath -lcviruntime -lz -lm -L$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/ive/lib -lcvi_ive_tpu -L$(SDK_PATH)/cvitek_tdl_sdk/lib -lcvi_tdl -L$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/lib -lpthread -latomic -o ./bin/sample_yolov8 ./bin/sample_yolov8.o
	rm ./bin/sample_yolov8.o

clean:
	rm -r bin