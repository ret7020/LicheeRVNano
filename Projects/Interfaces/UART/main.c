#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define UART_PATH "/dev/ttyS2"

int main()
{
	printf("UART test %s\n", UART_PATH);
	char serialPort[] = UART_PATH;
	char txBuf[] = "abcd";
	struct termios tty;
	ssize_t writeLen;
	int serialFd;
	char rxBuffer[256];
	int bytesRead;

	serialFd = open(serialPort, O_RDWR | O_NOCTTY);
	printf("%d\n", serialFd);

	memset(&tty, 0, sizeof(tty));

	// Setting baud
	cfsetospeed(&tty, B115200);
	cfsetispeed(&tty, B115200);

	// Generic flags
	tty.c_cflag &= ~PARENB;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;

	while (1)
	{
		writeLen = write(serialFd, txBuf, sizeof(txBuf));
		printf("%d\n", writeLen);
		usleep(50000);
	}

	return 0;
}
