build:
	mkdir -p bin
	echo Using: ${COMPILER}
	${COMPILER}/riscv64-unknown-linux-musl-gcc main.c -o ./bin/i2c_aht20 

deploy:
	scp ./bin/i2c_aht20 root@${LICHEERV_IP}:/root/i2c_ah20
