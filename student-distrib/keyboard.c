#include "keyboard.h"

void keyboard_init() {
    idt[33].present = 1;
    SET_IDT_ENTRY(idt[33], keyboard__handler);

    enable_irq(1);
}

void keyboard__handler() {
    unsigned char status;
    unsigned char key;

    save_registers();
    cli();

    send_eoi(KBD_IRQ);
    status = inb(KBD_STATUS_PORT);

    //if(status & 0x1) {
        key = inb(KBD_DATA_PORT);
        putc(key);
    //}

    sti();
    restore_registers();
}
