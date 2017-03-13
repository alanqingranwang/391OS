/* JC
 * keyboard.c - contains function to initialize the keyboard IRQ 1
 *              also contains the handler for keyboard input.
 * tab size = 2, spaces
 */

#include "keyboard.h"

/* AW
 * This table contains the corresponding character's scan number from
 *  in the kbd_ascii_key_map.
 */
static unsigned char kbd_scan_code[TOTAL_SCANCODES] =
{
  0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17,
  0x24, 0x25, 0x26, 0x32, 0x31, 0x18, 0x19, 0x10, 0x13,
  0x1F, 0x14, 0x16, 0x2F, 0x11, 0x2D, 0x15, 0x2C, 0x0B,
  0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A
};

/* AW
 * This table contains the character associated with the scan number from
 *  the kbd_scan_code.
 */
static unsigned char kbd_ascii_key_map[TOTAL_SCANCODES] =
{
  'a','b','c','d','e','f','g','h','i',
  'j','k','l','m','n','o','p','q','r',
  's','t','u','v','w','x','y','z','0',
  '1','2','3','4','5','6','7','8','9'
};

/* AW
 * keyboard_init
 *      DESCRIPTION:
 *          Enable the PIC IRQ 1 to allow keyboard interrupts
 *      INPUT: none
 *      OUTPUT: none
 *      RETURN VALUE: none
 *      SIDE EFFECTS: none
 */
void keyboard_init()
{
    uint32_t flags;
    cli_and_save(flags);

    // set the IDT table entry for KBD
    // Map keyboard interrupts to IDT
    idt[33].present = 1;
    SET_IDT_ENTRY(idt[33], keyboard_handler);    

    enable_irq(KBD_IRQ);    // enable IRQ 1
    restore_flags(flags);
}

/* AW
 * keyboard_handler
 *      DESCRIPTION:
 *          Handles interrupts from the keyboard.
 *          When the keyboard imputs a character, the handler occurs and Handles
 *              the situation. If it's a valid character, the function will output
 *              the character to the screen.
 *      INPUT: none
 *      OUTPUT: the inputed character from the keyboard
 *      RETURN VALUE: none
 *      SIDE EFFECTS: none
 */
void keyboard_handler()
{
    // save registers, assembly wrapping
    save_registers();
    uint32_t flags;
    // save previous state of interrupts, and prevent them
    cli_and_save(flags);
    send_eoi(KBD_IRQ);  // tell PIC to continue with its' work

    int32_t i;
    // get input key
    uint8_t key;
    key = inb(KBD_DATA_PORT);

    // hit grave key to invoke test_interrupts
    // can replace the call with rtc_init and remove the init from kernel
    // add additional code to rtc_handler
    if(key == GRAVE_SCAN_CODE) {
        test_interrupts();
    }

    // if it's within the given range search the table for char
    else if(key >= ABC_LOW_SCANS && key <= ABC_HIGH_SCANS) {
        i = 0;
        while(1) { // find the scancode for the character
            if(key == kbd_scan_code[i])
                break;
            i++;
        }
        printf("%c", kbd_ascii_key_map[i]); // print the character
    }

    // restore and return
    restore_flags(flags);
    restore_registers();
}
