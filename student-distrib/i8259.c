/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void
i8259_init(void)
{
    uint32_t flags; // uint32_t suppose to be unsigned long
	cli_and_save(flags); // save original flag state and clear interrupts
	// assume that all port writes are immediate
	// master init
	outb(ICW1, MASTER_8259_PORT);		// ICW1: select 8259A-1 init
	outb(ICW2_MASTER, MASTER_MD);	 	// ICW2: 8259A-1 IR0-7 mapped to 0x20 - 0x27
	outb(ICW3_MASTER, MASTER_MD);	 	// 8259A-1 (the master) has a slave on IR2
	outb(ICW4, MASTER_MD);
	// slave init
	outb(ICW1, SLAVE_8259_PORT);		// ICW1: select 8259A-2 init
	outb(ICW2_SLAVE, SLAVE_MD);		// ICW2: 8259-2 IR0-7 mapped to 0x28 - 0x2F
	outb(ICW3_SLAVE, SLAVE_MD);		// 8259A-2 is a slave on master's IR2
	outb(ICW4, SLAVE_MD);				// (slave's support for AEOI in flat mode is to be investigated)
	// initalize all IRQ to off, PIC is active low
	master_mask = BYTE_MASK;
	slave_mask = BYTE_MASK;
	// mask the 0x21 and 0xA1 mask/data registers
	outb(BYTE_MASK, MASTER_MD);
	outb(BYTE_MASK, SLAVE_MD);
	// enable slave IRQ 2
	enable_irq(SLAVE_IRQ);
	restore_flags(flags); // restore the flags originally set
}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{
    if(irq_num < 8)
	{
		// takes the data from the port, and turns on the given irq_num
		master_mask &= ~(1 << irq_num);
		outb(master_mask, MASTER_MD);
	}
	else
	{
		irq_num -= 8; // reduce the irq_num to be in range
		// takes the data from the port, and turns on the given irq_num
		slave_mask &= ~(1 << irq_num);
		outb(slave_mask, SLAVE_MD);
	}
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
    if(irq_num < 8)
	{
		// takes the data from the port, and turns on the given irq_num
		master_mask |= (1 << irq_num);
		outb(master_mask, MASTER_MD);
	}
	else
	{
		irq_num -= 8; // reduce the irq_num to be in range
		// takes the data from the port, and turns on the given irq_num
		slave_mask |= (1 << irq_num);
		outb(slave_mask, SLAVE_MD);
	}
}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
    if(irq_num >= 8) // slave interrupts
	{
		outb((EOI | SLAVE_IRQ), MASTER_8259_PORT);	// tell master that slave is done too
		outb((EOI | (irq_num-8)), SLAVE_8259_PORT); 	// send end of interrupt to slave
	}
	else
		outb((EOI | irq_num), MASTER_8259_PORT); 		// send end of interrupt to master
}
