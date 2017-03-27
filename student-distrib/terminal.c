#include "terminal.h"

int32_t terminal_dipatcher(int cmd, op_data_t input){
	switch(cmd){
		case OPEN:
			return terminal_open();
		case CLOSE:
			return terminal_close(input.fd);
		case READ:
			return terminal_read(input.fd, (int8_t*)(input.buf), input.nbytes);
		case WRITE:
			return terminal_write(input.fd, (int8_t*)(input.buf), input.nbytes);
		default:
			return -1;
	}
}

int32_t terminal_open(){
	return 0;
}

int32_t terminal_close(int32_t fd){
	return 0;
}

int32_t terminal_read(int32_t fd, int8_t* buf, int32_t nbytes){
	return -1;
}

int32_t terminal_write(int32_t fd, int8_t* buf, int32_t nbytes){
	int32_t success = 0;

	int32_t i;
	for(i = 0; i < nbytes; i++){
		putc(buf[i]);
		success++;
	}
	return success;

	return -1;
}
