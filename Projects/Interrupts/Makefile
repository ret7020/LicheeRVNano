build:
	mkdir -p bin
	echo Using: ${COMPILER}
	${COMPILER}/riscv64-unknown-linux-musl-gcc main.c -o ./bin/interrupt_handler 

deploy:
	scp ./bin/interrupt_handler root@${LICHEERV_IP}:/root/interrupt_handler
