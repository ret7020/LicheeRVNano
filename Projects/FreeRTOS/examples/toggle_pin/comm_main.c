/* Standard includes. */
#include <stdio.h>
#include <stdint.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "mmio.h"
#include "delay.h"

/* cvitek includes. */
#include "printf.h"
#include "rtos_cmdqu.h"
#include "cvi_mailbox.h"
#include "intr_conf.h"
#include "top_reg.h"
#include "memmap.h"
#include "comm.h"
#include "cvi_spinlock.h"

//#define __DEBUG__

#ifdef __DEBUG__
#define debug_printf printf
#else
#define debug_printf(...)
#endif

/****************************************************************************
 * Function prototypes
 ****************************************************************************/
void app_task(void *param);
void pinMode(uint8_t port, uint8_t pin, uint8_t value);
void writePin(uint8_t port, uint8_t pin, uint8_t value);
uint8_t readPin(uint8_t port, uint8_t pin);

/****************************************************************************
 * Global parameters
 ****************************************************************************/
#define TOP_BASE 0x03000000

// GPIO Register base
#define XGPIO (TOP_BASE + 0x20000)
// GPIO Port offset
#define GPIO_SIZE 0x1000

// Port A external port register (read from here when configured as input)
#define GPIO_EXT_PORTA 0x50
// Port A data register
#define GPIO_SWPORTA_DR 0x00
// Port A data direction register
#define GPIO_SWPORTA_DDR 0x04

#define PORT_A 0
#define PORT_B 1
#define PORT_C 2
#define PORT_D 3

#define GPIO_INPUT 0
#define GPIO_OUTPUT 1
#define GPIO_LOW 0
#define GPIO_HIGH 1

#define PINMUX_BASE (TOP_BASE + 0x1000)
#define FMUX_GPIO_FUNCSEL_BASE 0xd4
#define FMUX_GPIO_FUNCSEL_MASK 0x7
#define FMUX_GPIO_FUNCSEL_GPIOC 3

#define FUNCSEL(port, pin) (FMUX_GPIO_FUNCSEL_BASE + port * 0x20u + pin)
#define BIT(x) (1UL << (x))

/* mailbox parameters */
volatile struct mailbox_set_register *mbox_reg;
volatile struct mailbox_done_register *mbox_done_reg;
volatile unsigned long *mailbox_context; // mailbox buffer context is 64 Bytess

/****************************************************************************
 * Function definitions
 ****************************************************************************/

void pinMode(uint8_t port, uint8_t pin, uint8_t value)
{
	mmio_clrsetbits_32(XGPIO + GPIO_SIZE * port + GPIO_SWPORTA_DDR,
			   BIT(pin), // erase Bit of PIN_NO( LED )
			   BIT(pin) // set Bit of PIN_NO( LED )
	);
}

void writePin(uint8_t port, uint8_t pin, uint8_t value)
{
	uint32_t base_addr = XGPIO + GPIO_SIZE * port;

	uint32_t reg_val = mmio_read_32(base_addr + GPIO_SWPORTA_DR);
	reg_val = (value == GPIO_HIGH ? (reg_val | BIT(pin)) :
					(reg_val & (~BIT(pin))));
	mmio_write_32(base_addr + GPIO_SWPORTA_DR, reg_val);
}

uint8_t readPin(uint8_t port, uint8_t pin)
{
	uint32_t base_addr = XGPIO + GPIO_SIZE * port;
	uint32_t reg_val = 0;

	// let's find out if this pin is configured as GPIO and INPUT or OUTPUT
	uint32_t func = mmio_read_32(PINMUX_BASE + FUNCSEL(port, pin));
	if (func == FMUX_GPIO_FUNCSEL_GPIOC) {
		uint32_t dir = mmio_read_32(base_addr + GPIO_SWPORTA_DDR);
		if (dir & BIT(pin)) {
			reg_val = mmio_read_32(GPIO_SWPORTA_DR + base_addr);
		} else {
			reg_val = mmio_read_32(GPIO_EXT_PORTA + base_addr);
		}
	} else {
		printf("%d not configured as GPIO\n", pin);
	}

	return ((reg_val >> pin) & 1);
}

DEFINE_CVI_SPINLOCK(mailbox_lock, SPIN_MBOX);

void app_task_demo(void *param)
{
	uint8_t PIN = 15; // Addr 0x0300103C

	uint8_t PORT = PORT_A;
	mmio_write_32(0x0300103C, 0x3); // GPIOA 15 GPIO_MODE
	pinMode(PORT, PIN, GPIO_OUTPUT);

	printf("Port %d, pin %d\n", PORT, PIN);

	while (1) {
		writePin(PORT, PIN, GPIO_HIGH);
		vTaskDelay(50);

		writePin(PORT, PIN, GPIO_LOW);
		vTaskDelay(50);

		// printf("Hello RISC-V from the small core!\r\n");
	}
}

void main_cvirtos(void)
{
	printf("create cvi task\n");

	/* Start the tasks and timer running. */
	xTaskCreate(app_task_demo, "task_demo", 1024, NULL, 1, NULL);
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following
    line will never be reached.  If the following line does execute, then
    there was either insufficient FreeRTOS heap memory available for the idle
    and/or timer tasks to be created, or vTaskStartScheduler() was called from
    User mode.  See the memory management section on the FreeRTOS web site for
    more details on the FreeRTOS heap http://www.freertos.org/a00111.html.  The
    mode from which main() is called is set in the C start up code and must be
    a privileged mode (not user mode). */
	printf("cvi task end\n");

	for (;;)
		;
}
