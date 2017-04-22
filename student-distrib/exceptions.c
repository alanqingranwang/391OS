#include "exceptions.h"
#include "syscall.h"

/* map exception to proper handler function */
void exception_0() { exception_handler(0);}
void exception_1() { exception_handler(1);}
void exception_2() { exception_handler(2);}
void exception_3() { exception_handler(3);}
void exception_4() { exception_handler(4);}
void exception_5() { exception_handler(5);}
void exception_6() { exception_handler(6);}
void exception_7() { exception_handler(7);}
void exception_8() { exception_handler(8);}
void exception_9() { exception_handler(9);}
void exception_10() { exception_handler(10);}
void exception_11() { exception_handler(11);}
void exception_12() { exception_handler(12);}
void exception_13() { exception_handler(13);}
void exception_14() { exception_handler(14);}
void exception_15() { exception_handler(15);}
void exception_16() { exception_handler(16);}
void exception_17() { exception_handler(17);}
void exception_18() { exception_handler(18);}
void exception_19() { exception_handler(19);}
void exception_20() { exception_handler(20);}
void exception_21() { exception_handler(21);}
void exception_22() { exception_handler(22);}
void exception_23() { exception_handler(23);}
void exception_24() { exception_handler(24);}
void exception_25() { exception_handler(25);}
void exception_26() { exception_handler(26);}
void exception_27() { exception_handler(27);}
void exception_28() { exception_handler(28);}
void exception_29() { exception_handler(29);}
void exception_30() { exception_handler(30);}
void exception_31() { exception_handler(31);}

void print_cr2()
{
    unsigned long suspected_addr;
    asm volatile("movl %%cr2, %0 \n"
        : "=r" (suspected_addr)
    );
    printf("Suspected Address in CR2: %x\n", suspected_addr);
}

/*
 * exception_handler
 *   DESCRIPTION: Prints proper exception to blue screen of death
 *   INPUTS: i - determines which exception was reached
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Halts OS by looping infinitely. Displays blue
 *   screen of death.
 */
void exception_handler(int i) {
    clear();
    switch(i) {
        case 0:
            printf("Divide Error Exception\n");
            break;
        case 1:
            printf("Debug Exception\n");
            break;
        case 2:
            printf("NMI Interrupt\n");
            break;
        case 3:
            printf("Breakpoint Exception\n");
            break;
        case 4:
            printf("Overflow Exception\n");
            break;
        case 5:
            printf("BOUND Range Exceeded Exception\n");
            break;
        case 6:
            printf("Invalid Opcode Exception\n");
            break;
        case 7:
            printf("Device Not Available Exception\n");
            break;
        case 8:
            printf("Double Fault Exception\n");
            break;
        case 9:
            printf("Coprocessor Segment Overrun\n");
            break;
        case 10:
            printf("Invalid TSS Exception\n");
            break;
        case 11:
            printf("Segment Not Present\n");
            break;
        case 12:
            printf("Stack Fault Exception\n");
            break;
        case 13:
            printf("General Protection Exception\n");
            break;
        case 14:
            printf("Page-Fault Exception\n");
            print_cr2();
            break;
        case 15:
            printf("Reserved\n");
            break;
        case 16:
            printf("x87 FPU Floating-Point Error\n");
            break;
        case 17:
            printf("Alignment Check Exception\n");
            break;
        case 18:
            printf("Machine-Check Exception\n");
            break;
        case 19:
            printf("SIMD Floating-Point Exception\n");
            break;
        case 20:
            printf("Reserved\n");
            break;
        case 21:
            printf("Reserved\n");
            break;
        case 22:
            printf("Reserved\n");
            break;
        case 23:
            printf("Reserved\n");
            break;
        case 24:
            printf("Reserved\n");
            break;
        case 25:
            printf("Reserved\n");
            break;
        case 26:
            printf("Reserved\n");
            break;
        case 27:
            printf("Reserved\n");
            break;
        case 28:
            printf("Reserved\n");
            break;
        case 29:
            printf("Reserved\n");
            break;
        case 30:
            printf("Reserved\n");
            break;
        case 31:
            printf("Reserved\n");
            break;
    }

    exception_flag = 1;
    halt(255);
    // show_blue_screen();
    // while(1);
}
