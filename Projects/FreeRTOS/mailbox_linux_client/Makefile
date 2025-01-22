build:
	mkdir -p bin
	echo Using: ${COMPILER}
	${COMPILER}/riscv64-unknown-linux-musl-gcc main.c -o ./bin/mailbox

deploy:
	scp ./bin/mailbox root@${LICHEERV_IP}:/root/mailbox
