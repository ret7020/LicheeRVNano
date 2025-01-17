// INCLUDES
#include "hal_uart_dw.h"
/*
...
*/

// TASK source
void app_task_demo(void *param)
{
	// HAL UART1 test

	mmio_write_32(0x03001064, 0x6); // TX; UART1 on pins 18,19
	mmio_write_32(0x03001068, 0x6); // RX 0x03001068 0x6

	static struct dw_regs *uart = 0;
	int baudrate = 115200, uart_clock = 25 * 1000 * 1000;
	int divisor = (uart_clock + 8 * baudrate) / (16 * baudrate);

	uart = (struct dw_regs *)UART1_BASE;

	uart->lcr = uart->lcr | UART_LCR_DLAB | UART_LCR_8N1;
	uart->dll = divisor & 0xff;
	uart->dlm = (divisor >> 8) & 0xff;
	uart->lcr = uart->lcr & (~UART_LCR_DLAB);

	uart->ier = 0;
	uart->mcr = UART_MCRVAL;
	uart->fcr = UART_FCR_DEFVAL;

	uart->lcr = 3;

	while (1) {
		for (int i = 0; i < 10; i++) {
			while (!(uart->lsr & UART_LSR_THRE))
				;
			uart->rbr = 'Z';
		}
		vTaskDelay(200); // 1 second

	}
}
