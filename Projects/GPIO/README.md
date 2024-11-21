# GPIO test

## Digital pins


## PWM pins
You can use 2 pwm pins on the board: `PWM7` and `PWM6`, connected to `pwmchip4`. Use pwmTest.c to test (compile with: `make build_pwm`)


## Pin Map
| PIN NAME       | NUM     | ADDR       | val to enable |
| ----------     | ----    | ------     | ---           |
| PWM7           |  499    | 0x03001064 | 2             |
| PWM6           |  498    | 0x03001068 | 2             |
| UART2_TX       |  509    | 0x03001070 | 0x2           |
| UART2_RX       |  508    | 0x03001074 | 0x2           |


```bash
# Example with devmem:
# ./devmem addr b val

./devmem 0x030010D0 b 0x5
```