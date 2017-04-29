.globl keyboard_handler_wrapper
.globl rtc_handler_wrapper
.globl pit_handler_wrapper
.globl syscall_handler_wrapper
.globl user_context_switch
.globl sys_ret
.globl sys_ret_halt

# not in use yet
keyboard_handler_wrapper:
  pushal
  call keyboard_handler
  popal
  iret

# not in use yet
rtc_handler_wrapper:
  pushal
  call rtc_handler
  popal
  iret

pit_handler_wrapper:
  pushal
  call pit_handler
  popal
  iret

# JC
# syscall_handler
# 	DESCRIPTION:
#			Called when an interrupt 0x80.
#			Takes in a system call number and does the proper task.
#		INPUT: Parameters for each case is passed in via registers.
#				EAX - system number, described in header file.
#				EBX - first argument
#				ECX - second argument
#				EDX - third argument
#		OUTPUT:
#		RETURN VALUE: 0 if successful
#						 -1 failed.
#		SIDE EFFECTS:
#
#
syscall_handler_wrapper:
  # save registers
  pushl %edi
  pushl %esi
  pushl %edx
  pushl %ecx
  pushl %ebx
  # parameters
  pushl %edx # param 3
  pushl %ecx # param 2
  pushl %ebx # param 1

  # check if it's a valid call
  cmpl $10, %eax
  jae syscall_return_failure # if cmd >= 10
  cmpl $0, %eax
  jbe syscall_return_failure # if cmd <= 0
  call *dispatcher(, %eax, 4) # go to the proper function
  jmp sys_ret # return

syscall_return_failure:
  movl $-1, %eax # return -1

sys_ret:
  # remove the params
  popl %ebx
  popl %ecx
  popl %edx
  # restore the registers
  popl %ebx
  popl %ecx
  popl %edx
  popl %esi
  popl %edi
  iret

# function pointers
dispatcher: # 0 pad it
  .long def_cmd
  .long halt, execute, read, write, open, close
  .long getargs, vidmap, set_handler, sigreturn

user_context_switch:
  cli
  movl 4(%esp), %edx	# Retrieve input
  movl $0x2B, %eax      # USER_DS
  movw %ax,   %ds
  movw %ax,   %es
  movw %ax,   %fs
  movw %ax,   %gs

  pushl $0x2B     		 # USER_DS
  pushl $0x83FFFFC  	 # esp

  pushf
  popl  %ebx
  orl   $0x200, %ebx
  pushl %ebx

  pushl $0x23         	 # USER_CS
  pushl %edx      	 	 # eip
  iret
