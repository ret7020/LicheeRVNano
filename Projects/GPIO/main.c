#include <stdio.h>
#include "gpio.h"

int main(){
    int pinId = 0;
    int pass = 0;
    scanf("%d", &pinId);
    printf("Working with pin: %d\n", pinId);
    exportPin(pinId);
    setDirPin(pinId, "out");
    writePin(pinId, 1);
    printf("Write 1\n");
    scanf("%d", &pass);

    writePin(pinId, 0);
    printf("Write 0\n");
    unexportPin(pinId);

    

    return 0;
}