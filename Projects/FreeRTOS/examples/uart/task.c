void app_task_demo(void *param)
{
	uint8_t PIN = 15; // Addr 0x0300103C

	uint8_t PORT = PORT_A;
	mmio_write_32(0x0300103C, 0x3); // GPIOA 15 GPIO_MODE
	pinMode(PORT, PIN, GPIO_OUTPUT);

	printf("Port %d, pin %d\n", PORT, PIN);

	// HAL UART1 test

	mmio_write_32(0x03001064, 0x6); // TX TO USE UART1 TX pin on board
					// RX 0x03001064  0x6

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
		writePin(PORT, PIN, GPIO_HIGH);
		vTaskDelay(50);

		writePin(PORT, PIN, GPIO_LOW);
		vTaskDelay(50);

		for (int i = 0; i < 10; i++) {
			while (!(uart->lsr & UART_LSR_THRE))
				;
			uart->rbr = 'Z';
		}

		printf("Hello RISC-V from the small core!\r\n");
	}
}
