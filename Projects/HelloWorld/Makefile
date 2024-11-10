build:
	mkdir -p bin
	echo Using: ${COMPILER}
	${COMPILER}/riscv64-unknown-elf-gcc-10.2.0 main.c -o ./bin/hello 

deploy:
	scp ./bin/hello root@${LICHEERV_IP}:/root/hello
