build:
	mkdir -p bin
	echo Using: ${COMPILER}
	${COMPILER}/riscv64-unknown-linux-musl-gcc main.c -o ./bin/hello 

deploy:
	scp ./bin/hello root@${LICHEERV_IP}:/root/hello
