/* JC
 * keyboard.c - contains function to initialize the keyboard IRQ 1
 *              also contains the handler for keyboard input.
 * tab size = 2, spaces
 */

#include "keyboard.h"

/*
 * 0 - neither
 * 1 - shift
 * 2 - caps
 * 3 - shift and caps
 */
static int caps_shift_flag = 0;
static int ctrl_flag = 0;
static int screen_x_pos = 0;
static int buffer_index = 0;

static unsigned char buffer[128];

/* AW
 * This table contains the character associated with the scan number from
 *  the kbd_scan_code.
 */
static unsigned char kbd_ascii_key_map[4][TOTAL_SCANCODES] =
{
  {
   '\0',  '\0', '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
   '9', '0', '-', '=', '\0',	/* Backspace */
   '\0',			/* Tab */
   'q', 'w', 'e', 'r',	/* 19 */
   't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
   '\0',			/* 29   - Control */
   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
   '\'', '`', '\0',		/* Left shift */
   '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
   'm', ',', '.', '/', '\0',				/* Right shift */
   '*',
    '\0',	/* Alt */
  ' ',	/* Space bar */
    '\0',	/* Caps lock */
    '\0',	/* 59 - F1 key ... > */
    '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',
    '\0',	/* < ... F10 */
    '\0',	/* 69 - Num lock*/
    '\0',	/* Scroll Lock */
    '\0',	/* Home key */
    '\0',	/* Up Arrow */
    '\0',	/* Page Up */
  '-',
    '\0',	/* Left Arrow */
    '\0',
    '\0',	/* Right Arrow */
  '+',
    '\0',	/* 79 - End key*/
    '\0',	/* Down Arrow */
    '\0',	/* Page Down */
    '\0',	/* Insert Key */
    '\0',	/* Delete Key */
    '\0',   '\0',   '\0',
    '\0',	/* F11 Key */
    '\0',	/* F12 Key */
    '\0'	/* All other keys are undefined */
  },
// shift
  {
   '\0',  '\0', '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
   '(', ')', '_', '+', '\0',	/* Backspace */
   '\0',			/* Tab */
   'Q', 'W', 'E', 'R',	/* 19 */
   'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
   '\0',			/* 29   - Control */
   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
   '"', '~', '\0',		/* Left shift */
   '|', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
   'M', '<', '>', '?', '\0',				/* Right shift */
   '*',
    '\0',	/* Alt */
  ' ',	/* Space bar */
    '\0',	/* Caps lock */
    '\0',	/* 59 - F1 key ... > */
    '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',
    '\0',	/* < ... F1'\0' */
    '\0',	/* 69 - Num lock*/
    '\0',	/* Scroll Lock */
    '\0',	/* Home key */
    '\0',	/* Up Arrow */
    '\0',	/* Page Up */
  '-',
    '\0',	/* Left Arrow */
    '\0',
    '\0',	/* Right Arrow */
  '+',
    '\0',	/* 79 - End key*/
    '\0',	/* Down Arrow */
    '\0',	/* Page Down */
    '\0',	/* Insert Key */
    '\0',	/* Delete Key */
    '\0',   '\0',   '\0',
    '\0',	/* F11 Key */
    '\0',	/* F12 Key */
    '\0'	/* All other keys are undefined */
  },
// caps
  {
   '\0',  '\0', '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
   '9', '0', '-', '=', '\0',	/* Backspace */
   '\0',			/* Tab */
   'Q', 'W', 'E', 'R',	/* 19 */
   'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',	/* Enter key */
   '\0',			/* 29   - Control */
   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',	/* 39 */
   '\'', '`', '\0',		/* Left shift */
   '\\', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
   'M', ',', '.', '/', '\0',				/* Right shift */
   '*',
    '\0',	/* Alt */
  ' ',	/* Space bar */
    '\0',	/* Caps lock */
    '\0',	/* 59 - F1 key ... > */
    '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',
    '\0',	/* < ... F1'\0' */
    '\0',	/* 69 - Num lock*/
    '\0',	/* Scroll Lock */
    '\0',	/* Home key */
    '\0',	/* Up Arrow */
    '\0',	/* Page Up */
  '-',
    '\0',	/* Left Arrow */
    '\0',
    '\0',	/* Right Arrow */
  '+',
    '\0',	/* 79 - End key*/
    '\0',	/* Down Arrow */
    '\0',	/* Page Down */
    '\0',	/* Insert Key */
    '\0',	/* Delete Key */
    '\0',   '\0',   '\0',
    '\0',	/* F11 Key */
    '\0',	/* F12 Key */
    '\0'	/* All other keys are undefined */
  },
// shift and caps
  {
   '\0',  '\0', '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
   '(', ')', '_', '+', '\0',	/* Backspace */
   '\0',			/* Tab */
   'q', 'w', 'e', 'r',	/* 19 */
   't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
   '\0',			/* 29   - Control */
   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':',	/* 39 */
   '"', '~', '\0',		/* Left shift */
   '|', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
   'm', '<', '>', '?', '\0',				/* Right shift */
   '*',
    '\0',	/* Alt */
  ' ',	/* Space bar */
    '\0',	/* Caps lock */
    '\0',	/* 59 - F1 key ... > */
    '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',
    '\0',	/* < ... F1'\0' */
    '\0',	/* 69 - Num lock*/
    '\0',	/* Scroll Lock */
    '\0',	/* Home key */
    '\0',	/* Up Arrow */
    '\0',	/* Page Up */
  '-',
    '\0',	/* Left Arrow */
    '\0',
    '\0',	/* Right Arrow */
  '+',
    '\0',	/* 79 - End key*/
    '\0',	/* Down Arrow */
    '\0',	/* Page Down */
    '\0',	/* Insert Key */
    '\0',	/* Delete Key */
    '\0',   '\0',   '\0',
    '\0',	/* F11 Key */
    '\0',	/* F12 Key */
    '\0'	/* All other keys are undefined */
  }
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
    idt[KBD_VECTOR_NUM].present = 1;
    SET_IDT_ENTRY(idt[KBD_VECTOR_NUM], keyboard_handler);

    enable_irq(KBD_IRQ);    // enable IRQ 1
    restore_flags(flags);
}

/* AW
 * keyboard_handler
 *      DESCRIPTION:
 *          Handles interrupts from the keyboard.
 *          When the keyboard inputs a character, the handler occurs and Handles
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
    send_eoi(KBD_IRQ);  // tell PIC to continue with its work

    // get input key
    uint8_t key;
    key = inb(KBD_DATA_PORT);

    switch(key) {
        case CAPS:
            toggle_caps();
            break;
        case L_SHIFT_MAKE:
            toggle_shift(MAKE);
            break;
        case R_SHIFT_MAKE:
            toggle_shift(MAKE);
            break;
        case L_SHIFT_BREAK:
            toggle_shift(BREAK);
            break;
        case R_SHIFT_BREAK:
            toggle_shift(BREAK);
            break;
        case CTRL_MAKE:
            toggle_ctrl(MAKE);
            break;
        case CTRL_BREAK:
            toggle_ctrl(BREAK);
            break;
        case BKSP:
            handle_backspace();
            break;
        case ENTER:
            handle_enter();
            break;
        default:
            process_key(key);
            break;
    }

    // restore and return
    restore_flags(flags);
    restore_registers();
}

void toggle_caps() {
    if(caps_shift_flag == 0)
        caps_shift_flag = 2;
    else if(caps_shift_flag == 1)
        caps_shift_flag = 3;
    else if(caps_shift_flag == 2)
        caps_shift_flag = 0;
    else
        caps_shift_flag = 1;
}

void toggle_shift(int type) {
    if(type == MAKE) {
        if(caps_shift_flag == 0)
            caps_shift_flag = 1;
        else if(caps_shift_flag == 2)
            caps_shift_flag = 3;
    }
    else {
        if(caps_shift_flag == 1)
            caps_shift_flag = 0;
        else if(caps_shift_flag == 3)
            caps_shift_flag = 2;
    }
}

void toggle_ctrl(int type) {
    if(type == MAKE)
        ctrl_flag = 1;
    else
        ctrl_flag = 0;
}

void process_key(uint8_t key) {
    // if it's within the given range, search the table for char
    if(key >= ABC_LOW_SCANS && key <= ABC_HIGH_SCANS) {
        screen_x_pos++;
        if(key == L_MAKE && ctrl_flag) {
            clear();
            clear_buffer();
        }
        else if(screen_x_pos >= 80) {
            if(buffer_index + 1 < 128) {
                scroll();
                screen_x_pos = 0;
                putc(kbd_ascii_key_map[caps_shift_flag][key]); // print the character
                buffer[buffer_index] = key;
                buffer_index++;
            }
            else 
                screen_x_pos--;
        }
        else {
            if(buffer_index + 1 < 128) { // handle buffer overflow
                putc(kbd_ascii_key_map[caps_shift_flag][key]); // print the character
                buffer[buffer_index] = key;
                buffer_index++;
            }
            else
                screen_x_pos--;
        }
    }
}

void handle_backspace() {
    if(buffer_index - 1 >= 0) {
        backspace();
        buffer_index--;
        buffer[buffer_index] = ' ';
        screen_x_pos--;
    }
}

void handle_enter() {
    scroll();
    screen_x_pos = 0;
    clear_buffer();
}

void clear_buffer() {
    int i;
    for(i = 0; i < 128; i++) {
        buffer[i] = ' ';
    }
    buffer_index = 0;
    screen_x_pos = 0;
}
