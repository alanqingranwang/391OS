/* JC
 * keyboard.c - contains function to initialize the keyboard IRQ 1
 *              also contains the handler for keyboard input.
 * tab size = 2, spaces
 */
#include "keyboard.h"
#include "terminal.h"
#include "syscall.h"

#define EXIT_LEN 4

static int CTR4 = NUM_FREQ; // remove later
static int CTR3 = 0;
/*
 * 0 - neither
 * 1 - shift
 * 2 - caps
 * 3 - shift and caps
 */
static int caps_shift_flag[MAX_TERMINAL];
static int ctrl_flag;
static int alt_flag = 0;
static int buffer_index[MAX_TERMINAL];
static unsigned char buffer[MAX_TERMINAL][BUFFER_SIZE]; // three different keyboard buffers
static volatile int kbdr_flag = 0;

/***********************Keyboard Driver****************************/

/* AW
 * keyboard_open
 *   empty functionality, should always be open
 */
int32_t keyboard_open(const uint8_t* blank1) {
    return -1;
}

/* NM, JC
 * keyboard_read
 *  DESCRIPTION:
 *    Waits for the keyboard handler to be called, then takes the buffer
 *    that was inputed by the user and reads it.
 *  INPUT:
 *    fd - fd index, should be STDIN
 *    buf - buffer that we are putting stuff into
 *    nbytes - number of bytes that we are reading
 */
int32_t keyboard_read(int32_t fd, uint8_t* buf, int32_t nbytes) {
  int count;
  kbdr_flag = 0;

  if(buf == NULL)
    return -1;

  while(1){ // get the buffer
		if(kbdr_flag == 1){
			count = terminal_retrieve(buf, nbytes);
			break;
		}
  }

  // if the sting isn't the command exit, place the '\n' at the end
  if(strncmp((int8_t*)buf, "exit", EXIT_LEN) != 0) // if it's not exit then add this new line
  {
    buf[count] = '\n'; // replace the space with a new line
    count++;
  }
    // new line is specifically for programs such as hello
  return count;
}

/* AW
 *  keyboard_write
 *  empty functionality, you can't write to a keyboard.
 */
int32_t keyboard_write(int32_t fd, const void* blank1, int32_t blank2) {
    return -1;
}

/* AW
 * keyboard_close
 *  empty functionality, should always be open
 */
int32_t keyboard_close(int32_t fd) {
    return -1;
}

/**********************************************************************/

/* AW
 * This table contains the character associated with the scan number from
 *  the kbd_scan_code.
 */
static unsigned char kbd_ascii_key_map[KEY_MODES][TOTAL_SCANCODES] =
{
  {
   '\0',  '\0', '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
   '9', '0', '-', '=', '\0',  /* Backspace */
   '\0',      /* Tab */
   'q', 'w', 'e', 'r',  /* 19 */
   't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',  /* Enter key */
   '\0',      /* 29   - Control */
   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',  /* 39 */
   '\'', '`', '\0',   /* Left shift */
   '\\', 'z', 'x', 'c', 'v', 'b', 'n',      /* 49 */
   'm', ',', '.', '/', '\0',        /* Right shift */
   '*',
    '\0', /* Alt */
  ' ',  /* Space bar */
    '\0', /* Caps lock */
    '\0', /* 59 - F1 key ... > */
    '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',
    '\0', /* < ... F10 */
    '\0', /* 69 - Num lock*/
    '\0', /* Scroll Lock */
    '\0', /* Home key */
    '\0', /* Up Arrow */
    '\0', /* Page Up */
  '-',
    '\0', /* Left Arrow */
    '\0',
    '\0', /* Right Arrow */
  '+',
    '\0', /* 79 - End key*/
    '\0', /* Down Arrow */
    '\0', /* Page Down */
    '\0', /* Insert Key */
    '\0', /* Delete Key */
    '\0',   '\0',   '\0',
    '\0', /* F11 Key */
    '\0', /* F12 Key */
    '\0'  /* All other keys are undefined */
  },
// shift
  {
   '\0',  '\0', '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
   '(', ')', '_', '+', '\0',  /* Backspace */
   '\0',      /* Tab */
   'Q', 'W', 'E', 'R',  /* 19 */
   'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',  /* Enter key */
   '\0',      /* 29   - Control */
   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',  /* 39 */
   '"', '~', '\0',    /* Left shift */
   '|', 'Z', 'X', 'C', 'V', 'B', 'N',     /* 49 */
   'M', '<', '>', '?', '\0',        /* Right shift */
   '*',
    '\0', /* Alt */
  ' ',  /* Space bar */
    '\0', /* Caps lock */
    '\0', /* 59 - F1 key ... > */
    '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',
    '\0', /* < ... F1'\0' */
    '\0', /* 69 - Num lock*/
    '\0', /* Scroll Lock */
    '\0', /* Home key */
    '\0', /* Up Arrow */
    '\0', /* Page Up */
  '-',
    '\0', /* Left Arrow */
    '\0',
    '\0', /* Right Arrow */
  '+',
    '\0', /* 79 - End key*/
    '\0', /* Down Arrow */
    '\0', /* Page Down */
    '\0', /* Insert Key */
    '\0', /* Delete Key */
    '\0',   '\0',   '\0',
    '\0', /* F11 Key */
    '\0', /* F12 Key */
    '\0'  /* All other keys are undefined */
  },
// caps
  {
   '\0',  '\0', '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
   '9', '0', '-', '=', '\0',  /* Backspace */
   '\0',      /* Tab */
   'Q', 'W', 'E', 'R',  /* 19 */
   'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',  /* Enter key */
   '\0',      /* 29   - Control */
   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',  /* 39 */
   '\'', '`', '\0',   /* Left shift */
   '\\', 'Z', 'X', 'C', 'V', 'B', 'N',      /* 49 */
   'M', ',', '.', '/', '\0',        /* Right shift */
   '*',
    '\0', /* Alt */
  ' ',  /* Space bar */
    '\0', /* Caps lock */
    '\0', /* 59 - F1 key ... > */
    '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',
    '\0', /* < ... F1'\0' */
    '\0', /* 69 - Num lock*/
    '\0', /* Scroll Lock */
    '\0', /* Home key */
    '\0', /* Up Arrow */
    '\0', /* Page Up */
  '-',
    '\0', /* Left Arrow */
    '\0',
    '\0', /* Right Arrow */
  '+',
    '\0', /* 79 - End key*/
    '\0', /* Down Arrow */
    '\0', /* Page Down */
    '\0', /* Insert Key */
    '\0', /* Delete Key */
    '\0',   '\0',   '\0',
    '\0', /* F11 Key */
    '\0', /* F12 Key */
    '\0'  /* All other keys are undefined */
  },
// shift and caps
  {
   '\0',  '\0', '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
   '(', ')', '_', '+', '\0',  /* Backspace */
   '\0',      /* Tab */
   'q', 'w', 'e', 'r',  /* 19 */
   't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',  /* Enter key */
   '\0',      /* 29   - Control */
   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':',  /* 39 */
   '"', '~', '\0',    /* Left shift */
   '|', 'z', 'x', 'c', 'v', 'b', 'n',     /* 49 */
   'm', '<', '>', '?', '\0',        /* Right shift */
   '*',
    '\0', /* Alt */
  ' ',  /* Space bar */
    '\0', /* Caps lock */
    '\0', /* 59 - F1 key ... > */
    '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',
    '\0', /* < ... F1'\0' */
    '\0', /* 69 - Num lock*/
    '\0', /* Scroll Lock */
    '\0', /* Home key */
    '\0', /* Up Arrow */
    '\0', /* Page Up */
  '-',
    '\0', /* Left Arrow */
    '\0',
    '\0', /* Right Arrow */
  '+',
    '\0', /* 79 - End key*/
    '\0', /* Down Arrow */
    '\0', /* Page Down */
    '\0', /* Insert Key */
    '\0', /* Delete Key */
    '\0',   '\0',   '\0',
    '\0', /* F11 Key */
    '\0', /* F12 Key */
    '\0'  /* All other keys are undefined */
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
   SET_IDT_ENTRY(idt[KBD_VECTOR_NUM], keyboard_handler_wrapper);

   // clean the buffers
	int i, t; // char loop, terminal loop
	for(i = 0; i < BUFFER_SIZE; i++)
		for(t = 0; t < MAX_TERMINAL; t++)
			buffer[t][i] = '\0';

	// initialize all the flags
	for(t = 0; t < MAX_TERMINAL; t++)
	{
		buffer_index[t] = 0;
		caps_shift_flag[t] = NONE_MODE;
	}

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
    send_eoi(KBD_IRQ);  // tell PIC to continue with its work
    uint32_t flags;
    cli_and_save(flags);
    // get input key
    uint8_t key;
    key = inb(KBD_DATA_PORT);
    // handle all keystrokes
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
        case ALT:
            toggle_alt(MAKE);
            break;
        default:
            process_key(key);
            break;
    }
    restore_flags(flags);
}

/* AW
 * toggle_caps
 *      DESCRIPTION:
 *          Toggles the caps flag
 *      INPUT: none
 *      OUTPUT: none
 *      RETURN VALUE: none
 *      SIDE EFFECTS: none
 */
void toggle_caps() {
    if(caps_shift_flag[curr_terminal] == NONE_MODE)
        caps_shift_flag[curr_terminal] = CAPS_MODE;
    else if(caps_shift_flag[curr_terminal] == SHIFT_MODE)
        caps_shift_flag[curr_terminal] = SHIFT_CAPS_MODE;
    else if(caps_shift_flag[curr_terminal] == CAPS_MODE)
        caps_shift_flag[curr_terminal] = NONE_MODE;
    else
        caps_shift_flag[curr_terminal] = SHIFT_MODE;
}

/* AW
 * toggle_shift
 *      DESCRIPTION:
 *          Toggles the shift flag
 *      INPUT: type -- whether its a make or break stroke
 *      OUTPUT: none
 *      RETURN VALUE: none
 *      SIDE EFFECTS: none
 */
void toggle_shift(int type) {
    if(type == MAKE) {
        if(caps_shift_flag[curr_terminal] == NONE_MODE)
            caps_shift_flag[curr_terminal] = SHIFT_MODE;
        else if(caps_shift_flag[curr_terminal] == CAPS_MODE)
            caps_shift_flag[curr_terminal] = SHIFT_CAPS_MODE;
    }
    else {
        if(caps_shift_flag[curr_terminal] == SHIFT_MODE)
            caps_shift_flag[curr_terminal] = NONE_MODE;
        else if(caps_shift_flag[curr_terminal] == SHIFT_CAPS_MODE)
            caps_shift_flag[curr_terminal] = CAPS_MODE;
    }
}

/* AW
 * toggle_ctrl(int type)
 *      DESCRIPTION:
 *          Toggles the ctrl flag
 *      INPUT: type -- whether it's a make or break stroke
 *      OUTPUT: none
 *      RETURN VALUE: none
 *      SIDE EFFECTS: none
 */
void toggle_ctrl(int type) {
    if(type == MAKE)
        ctrl_flag = 1;
    else
        ctrl_flag = 0;
}

/*
 * toggle_alt
 *		DESCRIPTION:
 *			Toggles the alt flag when it's press
 *		INPUT:
 *			type - whether it's a make or break stroke
 *		OUTPUT: none
 *		RETURN VALUE: none
 */
void toggle_alt(int type) {
    if(type == MAKE)
        alt_flag = 1;
    else
        alt_flag = 0;
}

/* AW
 * process_key(uint8_t key)
 *      DESCRIPTION:
 *          Processes which key to display, manages buffer
 *      INPUT: key -- key to display
 *      OUTPUT: none
 *      RETURN VALUE: none
 *      SIDE EFFECTS: none
 */
void process_key(uint8_t key) {
    // if it's within the given range, search the table for char
    if(key >= ABC_LOW_SCANS && key <= ABC_HIGH_SCANS) {
        if(key == L_MAKE && ctrl_flag) {
            clear();
            int32_t i;
            printf("391OS> ");
            for(i = 0; (buffer[curr_terminal][i] != '\0') && i < BUFFER_SIZE; i++)
            	putc(buffer[curr_terminal][i]); // print the current buffer
        }
        else if(key == fn1 && alt_flag) { // f1
            terminal_switch(0);
        }
        else if(key == fn2 && alt_flag) { // f2
            terminal_switch(1);
        }
        else if(key == fn3 && alt_flag) { // f1
            terminal_switch(2);
        }
        /********Remove later************/
        // if pressed ctrl and 3s
        else if(key == THREE_SCAN && ctrl_flag){
            clear();
            test_file_data(CTR3); // print the next file in dentry
            CTR3++;
            CTR3 %= get_num_entries(); // wrap around
        }
        // if pressed ctrl and 4s
        else if(key == FOUR_SCAN && ctrl_flag)
        {
            clear();
            if(CTR4 == MAX_RATE-2) // keep it within the freq test
            {
              CTR4 = NUM_FREQ;
              print_freq(CTR4);
            }
            else{
              print_freq(CTR4); // show next freq
              CTR4--;
            }
        }
        // if pressed ctrl and 1s
        else if(key == ONE_SCAN && ctrl_flag)
        {
            clear(); // print file
            print_file_info();
        }

        /**************************/
        else if(buffer_index[curr_terminal] + 1 < BUFFER_SIZE) {
            putc(kbd_ascii_key_map[caps_shift_flag[curr_terminal]][key]); // print the character
            buffer[curr_terminal][buffer_index[curr_terminal]] = kbd_ascii_key_map[caps_shift_flag[curr_terminal]][key];
            buffer_index[curr_terminal]++;
        }
    }
}

/* AW
 * handle_backspace()
 *      DESCRIPTION:
 *          Manages backspace by calling helper function in lib.c
 *      INPUT: none
 *      OUTPUT: none
 *      RETURN VALUE: none
 *      SIDE EFFECTS: none
 */
void handle_backspace() {
    if(buffer_index[curr_terminal] - 1 >= 0) {
        backspace();
        buffer_index[curr_terminal]--;
        buffer[curr_terminal][buffer_index[curr_terminal]] = '\0';
    }
}

/* AW
 * handle_enter()
 *      DESCRIPTION:
 *          Manages enter keystroke
 *      INPUT: none
 *      OUTPUT: none
 *      RETURN VALUE: none
 *      SIDE EFFECTS: none
 */
void handle_enter() {
	kbdr_flag = 1;
   scroll();
   //call terminal read once implemented
   // call terminal read, save the buffer
   terminal_read(STDOUT_FD, (uint8_t*)buffer[curr_terminal], buffer_index[curr_terminal]);
   clear_buffer();
}

/* AW
 * clear_buffer()
 *      DESCRIPTION:
 *          clears the buffer
 *      INPUT: none
 *      OUTPUT: none
 *      RETURN VALUE: none
 *      SIDE EFFECTS: none
 */
void clear_buffer() {
    int i;
    for(i = 0; i < BUFFER_SIZE; i++) {
        buffer[curr_terminal][i] = '\0';
    }
    buffer_index[curr_terminal] = 0;
}
