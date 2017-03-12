/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled 
 *		Holds the most recent state of the masks
 */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* JC
 * i8259_init
 * 	DESCRIPTION: 
 *			This function initializes the 8259 PIC for both
 *			the Master (0x20) and Slave (0xA0) to assigned IRQ vector numbers.
 *			Master is assigned to 0x20 to 0x27
 *			Slave is assigned to 0x28 to 0x2F 
 * 	INPUT: none
 *		OUTPUT: 
 *		RETURN VALUE: void
 *		SIDE EFFECTS: changes master and slave PIC assignments
 *
 */
void
i8259_init(void)
{
	uint32_t flags; // uint32_t suppose to be unsigned long 
	cli_and_save(flags); // save original flag state and clear interrupts

	// mask the 0x21 and 0xA1 mask/data registers
	outb(BYTE_MASK, MASTER_MD);
	outb(BYTE_MASK, SLAVE_MD);

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

	// enable slave IRQ 2
	enable_irq(SLAVE_IRQ);

	restore_flags(flags); // restore the flags originally set
}

/* JC
 * enabling/disabling irq details:
 *		PIC has a mask register (IMR), 8 bits wide. It contains a bitmap
 *			of the request lines going into the PIC. When a bit is set, the PIC ignores the
 *			request and continues normal operation.
 *		Setting the mask on a higher IRQ will not affect a lower IRQ.
 *		Masking IRQ2 will cause the slave PIC to stop raising interrupts.
 */

/* JC
 * enable_irq
 *		http://wiki.osdev.org/8259_PIC#Masking
 * 	DESCRIPTION: 
 *			THIS SHOULD BE CALLED WHEN A NEW IRQ NEEDS TO BE ADDED.
 *			This function takes a given IRQ line and re-enables it.
 *			This unmasking allows the IRQ line to raise interrupts.
 * 	INPUT: irq_num - the interrupt on the PIC that we will be enabling
 *		OUTPUT: none
 *		RETURN VALUE: none
 *		SIDE EFFECTS: Allows the given irq_num to raise interrupts
 *
 */
void
enable_irq(uint32_t irq_num)
{
	if(irq_num < 8)
	{
		// takes the data from the port, and turns on the given irq_num
		master_mask = inb(MASTER_MD) & ~(1 << irq_num);
		outb(master_mask, MASTER_MD);
	}
	else
	{
		irq_num -= 8; // reduce the irq_num to be in range
		// takes the data from the port, and turns on the given irq_num
		slave_mask = inb(SLAVE_MD) & ~(1 << irq_num);
		outb(slave_mask, SLAVE_MD);
	}
}

/* JC
 * disable_irq
 *		http://wiki.osdev.org/8259_PIC#Masking
 * 	DESCRIPTION:
 *			This function takes a given IRQ line and disables it.
 *			This masking prevents the IRQ line to raise interrupts.
 * 	INPUT: irq_num - the interrupt on the PIC that we will be disabling
 *		OUTPUT: none
 *		RETURN VALUE: none
 *		SIDE EFFECTS: Prevents the given irq_num to raise interrupts
 *
 */
void
disable_irq(uint32_t irq_num)
{
	if(irq_num < 8)
	{
		// takes the data from the port, and turns on the given irq_num
		master_mask = inb(MASTER_MD) | (1 << irq_num);
		outb(master_mask, MASTER_MD);
	}
	else
	{
		irq_num -= 8; // reduce the irq_num to be in range
		// takes the data from the port, and turns on the given irq_num
		slave_mask = inb(SLAVE_MD) | (1 << irq_num);
		outb(slave_mask, SLAVE_MD);
	}
}

/*
EOIs may indicate the interrupt vector implicitly, or explicitly. An explicit EOI vector is indicated with the EOI,
whereas an implicit EOI vector will typically use a vector as indicated
by the PICs priority schema. For example, the highest vector in the ISR. Also, EOIs may be sent at the end of interrupt
processing by an interrupt handler, or the operation of a PIC may be set to auto-EOI at the start of the interrupt handler.
 */

/* Send end-of-interrupt signal for the specified IRQ */
/* JC
 * send_eoi
 *		http://wiki.osdev.org/8259_PIC#End_of_Interrupt
 * 	DESCRIPTION:
 *			Sends a signal to the PIC to indicate the completion of interrupt processing for a given interrupt.
 *			Used to cause a PIC to clear the corresponding bit in the In-Service Register, which allows more interrupt requests
 *				of equal or lower priority to be generated by the PIC.
 * 	INPUT: irq_num - the interrupt that is done
 *		OUTPUT: none
 *		RETURN VALUE: none
 *		SIDE EFFECTS: 
 */
void
send_eoi(uint32_t irq_num)
{
	if(irq_num >= 8) // slave interrupts
	{
		outb((EOI | (irq_num-8)), SLAVE_8259_PORT); 	// send end of interrupt to slave
		outb((EOI | SLAVE_IRQ), MASTER_8259_PORT);	// tell master that slave is done too
	}
	else
		outb((EOI | irq_num), MASTER_8259_PORT); 		// send end of interrupt to master
}

