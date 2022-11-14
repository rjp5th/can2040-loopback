#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"

#include "can2040.h"


static struct can2040 cbus;

struct can2040_msg heartbeat_msg = {
	.id = 0x27F,
	.dlc = 1,
	.data = {0}
};

static uint32_t notify_sticky = 0;
static bool notify_missed = false;
static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    if (notify_sticky == 0) {
        notify_sticky = notify;
    } else {
        notify_missed = true;
    }
}

static void __time_critical_func(pio_irq_handler)(void)
{
    can2040_pio_irq_handler(&cbus);
}

int main()
{
	stdio_init_all();

	puts("Startup");

	// Setup canbus
	can2040_setup(&cbus, 0);
	can2040_callback_config(&cbus, can2040_cb);

	irq_set_exclusive_handler(PIO0_IRQ_0, pio_irq_handler);
	irq_set_priority(PIO0_IRQ_0, 0);

	can2040_start(&cbus, clock_get_hz(clk_sys), 1000000, 4, 5);

	irq_set_enabled(PIO0_IRQ_0, true);	// IRQ enable after can2040_start to avoid small race condition

    // Setup timeouts
    absolute_time_t next_heartbeat = make_timeout_time_ms(10);

    while (1) {
        // Send heartbeat
        if (time_reached(next_heartbeat) && can2040_check_transmit(&cbus)) {
            puts("Sent heartbeat");

            can2040_transmit(&cbus, &heartbeat_msg);
            heartbeat_msg.data[0]++;
            next_heartbeat = make_timeout_time_ms(1000);
        }

        // Handle irq logging
        if (notify_sticky != 0) {
            printf("Notification received: %x\n", notify_sticky);
            notify_sticky = 0;
        }
        if (notify_missed) {
            puts("Notification missed");
            notify_missed = false;
        }
    }
}
