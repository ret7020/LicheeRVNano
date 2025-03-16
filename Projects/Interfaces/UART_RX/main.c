#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define UART_PATH "/dev/ttyS1"

/* If you use official OS img
    Configure system for UART usage
	devmem 0x03001068 b 0x6 # GPIOA 18 UART1 RX
	devmem 0x03001064 b 0x6 # GPIOA 19 UART1 TX

	You can get devmem tool from Tools/ directory of this repo
 */

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
	cfsetospeed(&tty, B57600);
	cfsetispeed(&tty, B57600);

	// Generic flags
	tty.c_cflag &= ~PARENB;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;

	while (1)
	{
		uint8_t buf[256];
		int len = read(serialFd, buf, sizeof(buf));
		if (len > 0) {}
		// usleep(50000);
	}

	return 0;
}
