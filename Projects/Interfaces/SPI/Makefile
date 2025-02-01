build:
	mkdir -p bin
	echo Using: ${COMPILER}
	${COMPILER}/riscv64-unknown-linux-musl-gcc main.c -o ./bin/spi_test 

deploy:
	scp ./bin/spi_test root@${LICHEERV_IP}:/root/spi_test
