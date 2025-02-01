#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

#define SPI_DEVICE_PATH "/dev/spidev2.0"

int main()
{
	int spi_file;
	uint8_t tx_buffer[5] = {1, 2, 3, 4, 5};
	uint8_t rx_buffer[5];

	// Open the SPI device
	if ((spi_file = open(SPI_DEVICE_PATH, O_RDWR)) < 0)
	{
		perror("Failed to open SPI device");
		return -1;
	}

	uint8_t mode = SPI_MODE_0;
	uint8_t bits = 8;
	if (ioctl(spi_file, SPI_IOC_WR_MODE, &mode) < 0)
	{
		perror("Failed to set SPI mode");
		close(spi_file);
		return -1;
	}

	struct spi_ioc_transfer transfer = {
		.tx_buf = (unsigned long)tx_buffer,
		.rx_buf = (unsigned long)rx_buffer,
		.len = sizeof(tx_buffer),
		.speed_hz = 1000000,  // SPI speed in Hz
		.delay_usecs = 0,
		.bits_per_word = 8,
	};
	
	if (ioctl(spi_file, SPI_IOC_MESSAGE(1), &transfer) < 0)
	{
		perror("Failed to perform SPI transfer");
		close(spi_file);
		return -1;
	}
	for (uint8_t i = 0; i < 5; i++) {printf("%d ", rx_buffer[i]);}
	printf("\n");


	close(spi_file);

	return 0;
}
