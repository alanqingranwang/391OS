/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

#include <linux/spinlock.h> // JC

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

static spinlock_t i8259_lock = SPIN_LOCK_UNLOCKED; // initialize to unlock

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
	spin_lock_irqsave(&i8259_lock, flags);

	// double check this
	// initialize masks to all high for IRQs
	master_mask = 0xff;
	slave_mask = 0xff;

	// mask the 0x21 and 0xA1 mask and data registers
	outb(master_mask, MASTER_MD);
	outb(slave_mask, MASTER_MD);

	// outb_p - this has to work on a wide range of PC hardware
	// master init
	outb_p(ICW1, MASTER_8259_PORT);		// ICW1: select 8259A-1 init
	outb_p(ICW2_MASTER, MASTER_MD);	 	// ICW2: 8259A-1 IR0-7 mapped to 0x20 - 0x27
	outb_p(ICW3_MASTER, MASTER_MD);	 	// 8259A-1 (the master) has a slave on IR2

	// slave init
	outb_p(ICW1, SLAVE_8259_PORT);		// ICW1: select 8259A-2 init
	outb_p(ICW2_SLAVE, SLAVE_MD);			// ICW2: 8259-2 IR0-7 mapped to 0x28 - 0x2F
	outb_p(ICW3_SLAVE, SLAVE_MD);			// 8259A-2 is a slave on master's IR2
	outb_p(ICW4, SLAVE_MD);					// (slave's support for AEOI in flat mode is to be investigated)

	// do I need this?
	udelay(100);								// wait for 8259A to initialize

	// double check this
	// restore the master and slave IRQ mask
	outb(master_mask, MASTER_MD);
	outb(slave_mask, SLAVE_MD);

	spin_unlock_irqrestore(&i8259_lock, flags);
}

/* Enable (unmask) the specified IRQ */
/* JC
 * enable_irq
 * 	DESCRIPTION: 
 * 	INPUT: irq_num - the interrupt on the PIC that we will be enabling
 *		OUTPUT: none
 *		RETURN VALUE: none
 *		SIDE EFFECTS:
 *
 */
void
enable_irq(uint32_t irq_num)
{

}

/* Disable (mask) the specified IRQ */
/* JC
 * disable_irq
 * 	DESCRIPTION:
 * 	INPUT: irq_num - the interrupt on the PIC that we will be disabling
 *		OUTPUT: none
 *		RETURN VALUE: none
 *		SIDE EFFECTS:
 *
 */
void
disable_irq(uint32_t irq_num)
{
}

/* Send end-of-interrupt signal for the specified IRQ */
/* JC
 * send_eoi
 * 	DESCRIPTION:
 * 	INPUT: 
 *		OUTPUT: none
 *		RETURN VALUE: none
 *		SIDE EFFECTS: 
 *
 */
void
send_eoi(uint32_t irq_num)
{
}

