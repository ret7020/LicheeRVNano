#ifndef GPIO_H
#define GPIO_H

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

/* Pins mapping
Default buildroot have 4 controllers for channels A, B, P
A - 480

Next map using gpio digital pins, that can be controlled from official buildroot (tested)
*/
#define GPIOA_15 495
#define GPIOA_24 504
#define GPIOA_22 502


int exportPin(int pin)
{
    FILE *exportFile = fopen("/sys/class/gpio/export", "w");
    if (exportFile == NULL)
        return -1;
    fprintf(exportFile, "%d", pin);
    fclose(exportFile);

    return 0;
}

int unexportPin(int pin)
{
    FILE *exportFile = fopen("/sys/class/gpio/unexport", "w");
    if (exportFile == NULL)
        return -1;
    fprintf(exportFile, "%d", pin);
    fclose(exportFile);

    return 0;
}

int setDirPin(int pin, const char *dir)
{
    char directionPath[50];
    snprintf(directionPath, sizeof(directionPath), "/sys/class/gpio/gpio%d/direction", pin);
    FILE *directionFile = fopen(directionPath, "w");
    if (directionFile == NULL)
        return -1;
    fprintf(directionFile, dir);
    fclose(directionFile);

    return 0;
}

int writePin(int pin, int value)
{
    char valuePath[50];
    snprintf(valuePath, sizeof(valuePath), "/sys/class/gpio/gpio%d/value", pin);
    FILE *valueFile = fopen(valuePath, "w");
    if (valueFile == NULL)
        return -1;

    if (value)
        fprintf(valueFile, "1");
    else
        fprintf(valueFile, "0");
    fflush(valueFile);
    fclose(valueFile);

    return 0;
}

int setInterruptType(int pin, const char *type)
{
    char interruptPath[50];
    snprintf(interruptPath, sizeof(interruptPath), "/sys/class/gpio/gpio%d/edge", pin);
    FILE *interruptFile = fopen(interruptPath, "w");
    if (interruptFile == NULL)
        return -1;
    fprintf(interruptFile, type);
    fclose(interruptFile);

    return 0;
}

int releasePin(int pin)
{
    FILE *unexportFile = fopen("/sys/class/gpio/unexport", "w");
    if (unexportFile == NULL)
        return -1;
    fprintf(unexportFile, "%d", pin);
    fclose(unexportFile);

    return 0;
}

int setPin(int pin, int value)
{
    // If you need to set pin value once use this function,
    // but if you need continuously pin writing manualy setup pin (exportPin, setDirPin) then call write and at the end releasePins
    return exportPin(pin) + setDirPin(pin, "out") + writePin(pin, value) + releasePin(pin);
}

int pwmExport(int pin){
    FILE *exportFile = fopen("/sys/class/pwm/pwmchip4/export", "w");
    if (exportFile == NULL)
        return -1;
    fprintf(exportFile, "%d", pin);
    fclose(exportFile);

    return 0;

}

int pwmSetParam(int pin, int val, int type){
    // type == 0: set pwm period
    // type == 1: set pwm duty cycle
    // type == 2: set enable/disable (1/0)
    char typeMap[3][15] = {"period", "duty_cycle", "enable"};
    char valPath[50];
    snprintf(valPath, sizeof(valPath), "/sys/class/pwm/pwmchip4/pwm%d/%s", pin, typeMap[type]);
    printf("Path: %s\n", valPath);

    FILE *exportFile = fopen(valPath, "w");
    if (exportFile == NULL)
        return -1;
    fprintf(exportFile, "%d", val);
    fclose(exportFile);

    return 0;
}

int pwmUnexport(int pin){
    FILE *exportFile = fopen("/sys/class/pwm/pwmchip4/unexport", "w");
    if (exportFile == NULL)
        return -1;
    fprintf(exportFile, "%d", pin);
    fclose(exportFile);

    return 0;

}

#endif
