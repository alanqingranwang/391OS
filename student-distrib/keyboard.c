#include "keyboard.h"

/* AW
 * This table contains the corresponding character's scan number from
 *  in the kbd_ascii_key_map.
 */
static unsigned char kbd_scan_code[36] =
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
static unsigned char kbd_ascii_key_map[36] =
{
  'a','b','c','d','e','f','g','h','i',
  'j','k','l','m','n','o','p','q','r',
  's','t','u','v','w','x','y','z','0',
  '1','2','3','4','5','6','7','8','9'
};

/* AW
 * keyboard_init
 *      DESCRIPTION:
 *      INPUT:
 *      OUTPUT:
 *      RETURN VALUE:
 */
void keyboard_init() {
    enable_irq(1);
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
void keyboard_handler() {
    save_registers();
    uint32_t flags;
    cli_and_save(flags);

    send_eoi(KBD_IRQ);

    uint8_t key;
    int32_t i;
    key = inb(KBD_DATA_PORT);

    if(key >= 0x02 && key <= 0x32) {
        i = 0;
        while(1) {
            if(key == kbd_scan_code[i])
                break;
            i++;
        }
        printf("%c", kbd_ascii_key_map[i]);
    }

    restore_flags(flags);
    restore_registers();
}
