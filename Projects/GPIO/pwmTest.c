#include <stdio.h>
#include "gpio.h"

int main(){
    int pin = 0, period = 0, dutyCycle = 0; 
    printf("Pin (PWM_X, supported: 3 (pwm7) and 2(pwm6)): \n");
    scanf("%d", &pin);
    printf("Period and duty cycle: \n");
    scanf("%d %d", &period, &dutyCycle);
    printf("Export status: %d\n", pwmExport(pin));
    printf("Setting period: %d\n", pwmSetParam(pin, period, 0));
    printf("Setting duty cycle: %d\n", pwmSetParam(pin, dutyCycle, 1));
    printf("Setting enable: %d\n", pwmSetParam(pin, 1, 2));
    pwmUnexport(pin);

    return 0;
}
