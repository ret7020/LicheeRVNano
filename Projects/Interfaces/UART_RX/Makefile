build:
	mkdir -p bin
	echo Using: ${COMPILER}
	${COMPILER}/riscv64-unknown-linux-musl-g++ main.c -o ./bin/uart_rx

deploy:
	scp ./bin/uart_rx root@${LICHEERV_IP}:/root/uart_rx
