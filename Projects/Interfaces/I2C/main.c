#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <time.h>

#define I2C_BUS "/dev/i2c-1"  // bus
#define AHT20_ADDR 0x38       // AHT20 I2C address

#define CMD_TRIGGER_MEASURE 0xAC
#define CMD_INIT 0xBE
#define CMD_SOFT_RESET 0xBA

int open_i2c_device() {
    int file = open(I2C_BUS, O_RDWR);
    if (file < 0) {
        exit(1);
    }
    if (ioctl(file, I2C_SLAVE, AHT20_ADDR) < 0) {
        exit(1);
    }
    return file;
}

void aht20_reset(int file) {
    uint8_t cmd = CMD_SOFT_RESET;
    if (write(file, &cmd, 1) != 1) {
        exit(1);
    }
    usleep(20000); 
}

void aht20_init(int file) {
    uint8_t cmd[3] = {CMD_INIT, 0x08, 0x00};
    if (write(file, cmd, 3) != 3) {
        exit(1);
    }
    usleep(10000);
}

void aht20_start_measurement(int file) {
    uint8_t cmd[3] = {CMD_TRIGGER_MEASURE, 0x33, 0x00};
    if (write(file, cmd, 3) != 3) {
        exit(1);
    }
    usleep(80000);
}

uint32_t aht20_read_raw_temperature(int file) {
    uint8_t data[6];

    if (read(file, data, 6) != 6) {
        exit(1);
    }
    uint32_t raw_temp = ((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5];

    return raw_temp;
}

float convert_temperature(uint32_t raw_temp) {
    return ((raw_temp * 200.0) / 1048576.0) - 50.0;
}

int main() {
    int file = open_i2c_device();

    aht20_reset(file);
    aht20_init(file);

    aht20_start_measurement(file);

    uint32_t raw_temp = aht20_read_raw_temperature(file);
    float temperature = convert_temperature(raw_temp);

    printf("Temperature: %.2f°C\n", temperature);

    close(file);
    return 0;
}

