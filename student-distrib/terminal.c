// #include "terminal.h"
//
// int terminal_dipatcher(op_data_t input, int cmd){
// 	switch(cmd){
// 		case OPEN: return terminal_open(input.filename);
// 		case CLOSE: return terminal_close(input.fd);
// 		case READ: return terminal_read(input.fd, input.buf, input.nbytes);
// 		case WRITE: return terminal_write(input.fd, input.buf, input.nbytes);
// 		default: return -1;
// 	}
// }
//
// int terminal_open(uint8_t *filename){
// 	return 0;
// }
//
// int terminal_close(int32_t fd){
// 	return 0;
// }
//
// int terminal_read(int32_t fd, void* buf, int32_t nbytes){
// 	return -1;
// }
//
// int terminal_write(int32_t fd, void* buf, int32_t nbytes){
// 	int success = 0;
//
// 	for(i = 0; i < nbytes; i++){
// 		putc(buf[i]);
// 		success++;
// 	}
// 	return success;
//
// 	return -1;
// }
