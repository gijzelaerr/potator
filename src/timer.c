#include "timer.h"

static uint8 timer_regs[2];
static int32 timer_cycles;
static BOOL timer_activated;

void timer_init() {
    printf( "timer: init\n");
}

void timer_done() {
    printf( "timer: done\n");
}

void timer_reset() {
    printf( "timer: reset\n");
    timer_regs[0] = 0x00;
    timer_regs[1] = 0x00;
    // timer_regs[2] = 0x00;  warning: array index 2 is past the end of the array (which contains 2 elements) [-Warray-bounds]

    timer_cycles = 0;
    timer_activated = FALSE;
}

void timer_write(uint32 addr, uint8 data) {
    //iprintf("timer: writing 0x%.2x at 0x%.4x\n", data, addr);
    timer_regs[addr & 0x01] = data;

    if (addr & 0x01) {
        timer_cycles = ((uint32) data) * 256;
        timer_activated = TRUE;
        timer_regs[0] &= 0xfe;
    }
}

uint8 timer_read(uint32 addr) {
    return (timer_regs[addr & 0x01]);
}

void timer_exec(uint32 cycles) {
    if (timer_activated) {
        timer_cycles -= cycles;

        if (timer_cycles < 0) {
            timer_regs[0] |= 0x01;
//			printf( "timer: irq\n");
            interrupts_irq();
            timer_activated = FALSE;
        }
    }
}
