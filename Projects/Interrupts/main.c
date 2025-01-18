#include <stdio.h>
#include <poll.h>
#include <pthread.h>
#include "gpio.h"

int main()
{
	exportPin(GPIOA_15);
	setDirPin(GPIOA_15, "in");
	setInterruptType(GPIOA_15, "rising");

	char valuePath[50];
	snprintf(valuePath, sizeof(valuePath), "/sys/class/gpio/gpio%d/value", GPIOA_15);
	int fd = open(valuePath, O_RDONLY | O_NONBLOCK);
	if (fd < 0)
	{
		perror("Unable to open GPIO value file");
		return 1;
	}
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLPRI;

	// Read first 'init' interrupt
	poll(&pfd, 1, -1);
	lseek(fd, 0, SEEK_SET);
	char tmp[3];
	read(fd, tmp, sizeof(tmp));

	while (1)
	{
		int ret = poll(&pfd, 1, -1);
		if (ret > 0)
		{
			char buf[3];
			lseek(fd, 0, SEEK_SET);
			read(fd, buf, sizeof(buf));
			printf("Interrupt detected!\n");
		}
	}

	close(fd);

	return 0;
}
