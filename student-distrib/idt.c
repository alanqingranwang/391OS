#include "idt.h"
#include "rtc.h"
#include "lib.h"

void idt_init() {
    lidt(idt_desc_ptr);

    SET_IDT_ENTRY(idt[0], exception_0);
    SET_IDT_ENTRY(idt[1], exception_1);
    SET_IDT_ENTRY(idt[2], exception_2);
    SET_IDT_ENTRY(idt[3], exception_3);
    SET_IDT_ENTRY(idt[4], exception_4);
    SET_IDT_ENTRY(idt[5], exception_5);
    SET_IDT_ENTRY(idt[6], exception_6);
    SET_IDT_ENTRY(idt[7], exception_7);
    SET_IDT_ENTRY(idt[8], exception_8);
    SET_IDT_ENTRY(idt[9], exception_9);
    SET_IDT_ENTRY(idt[10], exception_10);
    SET_IDT_ENTRY(idt[11], exception_11);
    SET_IDT_ENTRY(idt[12], exception_12);
    SET_IDT_ENTRY(idt[13], exception_13);
    SET_IDT_ENTRY(idt[14], exception_14);
    SET_IDT_ENTRY(idt[15], exception_15);
    SET_IDT_ENTRY(idt[16], exception_16);
    SET_IDT_ENTRY(idt[17], exception_17);
    SET_IDT_ENTRY(idt[18], exception_18);
    SET_IDT_ENTRY(idt[19], exception_19);
    SET_IDT_ENTRY(idt[20], exception_20);
    SET_IDT_ENTRY(idt[21], exception_21);
    SET_IDT_ENTRY(idt[22], exception_22);
    SET_IDT_ENTRY(idt[23], exception_23);
    SET_IDT_ENTRY(idt[24], exception_24);
    SET_IDT_ENTRY(idt[25], exception_25);
    SET_IDT_ENTRY(idt[26], exception_26);
    SET_IDT_ENTRY(idt[27], exception_27);
    SET_IDT_ENTRY(idt[28], exception_28);
    SET_IDT_ENTRY(idt[29], exception_29);
    SET_IDT_ENTRY(idt[30], exception_30);
    SET_IDT_ENTRY(idt[31], exception_31);

    int i;
    for(i = 0; i < 32; i++ ) {
        idt[i].present = 1;
        idt[i].dpl = 0;
        idt[i].reserved0 = 0;
        idt[i].size = 1;
        idt[i].reserved1 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved3 = 0;
        idt[i].reserved4 = 0;
        idt[i].seg_selector = KERNEL_CS;
    }

    for(i = 32; i < 256; i++) {
        SET_IDT_ENTRY(idt[i], 0);
        idt[i].present = 0;
        idt[i].dpl = 0;
        idt[i].reserved0 = 0;
        idt[i].size = 1;
        idt[i].reserved1 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved3 = 0;
        idt[i].reserved4 = 0;
		idt[i].seg_selector = KERNEL_CS;
    }

    // the RTC interrupt
    SET_IDT_ENTRY(idt[40], rtc_handler);
    idt[40].present = 1;
}
