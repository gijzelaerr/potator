
#include "m6502/m6502.h"
#include "interrupts.h"

void interrupts_init() {
    printf( "interrupts init\n");
}

void interrupts_reset() {
    printf( "interrupts reset\n");
}

void interrupts_done() {
    printf( "interrupts done\n");
}

void interrupts_write(uint32 Addr, uint8 data) {
}


uint8 interrupts_read(uint32 Addr) {
    return (0xff);
}


void interrupts_exec(uint32 cycles) {
}


void interrupts_nmi() {
    Int6502(supervision_get6502regs(), INT_NMI);
}

void interrupts_irq() {
    Int6502(supervision_get6502regs(), INT_IRQ);
}

