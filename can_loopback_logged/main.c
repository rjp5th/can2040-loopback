#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"

#include "can2040.h"
#include "trice/trice.h"


static struct can2040 cbus;

struct can2040_msg heartbeat_msg = {
	.id = 0x27F,
	.dlc = 1,
	.data = {0}
};

static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    if (notify == CAN2040_NOTIFY_RX) {
        TRICE( ID(12062),"MAIN_C: Notify RX\n");
    }
    if (notify == CAN2040_NOTIFY_TX) {
        TRICE( ID(14122),"MAIN_C: Notify TX\n");
    }
    if (notify == CAN2040_NOTIFY_ERROR) {
        TRICE( ID(13403),"MAIN_C: Notify ERROR\n");
    }
}

static void __time_critical_func(pio_irq_handler)(void)
{
    can2040_pio_irq_handler(&cbus);
}

int main()
{
	stdio_init_all();

	TRICE( ID(12417),"MAIN_C: Startup\n");

	// Setup canbus
	can2040_setup(&cbus, 0);
	can2040_callback_config(&cbus, can2040_cb);

	irq_set_exclusive_handler(PIO0_IRQ_0, pio_irq_handler);
	irq_set_priority(PIO0_IRQ_0, 0);

	can2040_start(&cbus, clock_get_hz(clk_sys), 1000000, 4, 5);

	irq_set_enabled(PIO0_IRQ_0, true);	// IRQ enable after can2040_start to avoid small race condition

    // Setup timeouts
    absolute_time_t next_heartbeat = make_timeout_time_ms(10);
    absolute_time_t next_transfer = get_absolute_time();

    while (1) {
        // Send heartbeat
        if (time_reached(next_heartbeat) && can2040_check_transmit(&cbus)) {
            TRICE( ID(15536),"MAIN_C: Sent heartbeat\n");
            can2040_transmit(&cbus, &heartbeat_msg);
            heartbeat_msg.data[0]++;
            next_heartbeat = make_timeout_time_ms(1000);
        }

        // Update logging
        if (time_reached(next_transfer)) {
            TriceTransfer();
            next_transfer = make_timeout_time_ms(20);
        }
    }
}
