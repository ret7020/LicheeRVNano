build:
	mkdir -p bin
	echo Using: ${COMPILER}
	${COMPILER}/riscv64-unknown-linux-musl-g++ -I$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/opencv/include -L$(SDK_PATH)/cvitek_tdl_sdk/sample/3rd/opencv/lib -lopencv_core -lopencv_imgproc -lopencv_imgcodecs main.c -o ./bin/otg_camera

deploy:
	scp ./bin/otg_camera root@${LICHEERV_IP}:/root/otg_camera
