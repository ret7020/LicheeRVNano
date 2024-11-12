build:
	mkdir -p bin
	echo Using: ${COMPILER}
	${COMPILER}/riscv64-unknown-linux-musl-g++ main.c -o ./bin/gpioTool 

deploy:
	scp ./bin/gpioTool root@${LICHEERV_IP}:/root/gpioTool
