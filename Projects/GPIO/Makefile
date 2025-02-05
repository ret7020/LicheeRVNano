build_digital:
	mkdir -p bin
	echo Using: ${COMPILER}
	${COMPILER}/riscv64-unknown-linux-musl-g++ digitalTest.c -o ./bin/gpioTool 

build_pwm: 
	mkdir -p bin
	echo Using: ${COMPILER}
	${COMPILER}/riscv64-unknown-linux-musl-g++ pwmTest.c -o ./bin/pwmTest 


deploy:
	scp ./bin/gpioTool root@${LICHEERV_IP}:/root/gpioTool
