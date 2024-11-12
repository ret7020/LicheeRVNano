build:
	mkdir -p bin
	echo Using: ${COMPILER}
	${COMPILER}/riscv64-unknown-linux-musl-g++ main.c -o ./bin/uart

deploy:
	scp ./bin/uart root@${LICHEERV_IP}:/root/uart
