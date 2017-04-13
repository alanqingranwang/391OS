.globl keyboard_handler_wrapper
.globl rtc_handler_wrapper
.globl syscall_handler_wrapper
.globl user_context_switch

keyboard_handler_wrapper:
  pusha
  call keyboard_handler
  popa
  iret

rtc_handler_wrapper:
  pusha
  call rtc_handler
  popa
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
  pusha

  pushl %edx
  pushl %ecx
  pushl %ebx
  jmp *dispatcher(, %eax, 4)

syscall_return_success:
  popa
  movl $0, %eax
  iret

syscall_return_failure:
  popa
  movl $-1, %eax
  iret

dispatcher:
  .long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn, default

user_context_switch:
  cli
  pushl %ebp
  movl %esp, %ebp
  movl 8(%ebp), %edx	# Retrieve input
  movl $0x2B, %eax      # USER_DS
  movw %ax,   %ds
  movw %ax,   %es
  movw %ax,   %fs
  movw %ax,   %gs

  pushl $0x2B     		 # USER_DS
  pushl $0x83FFFFC  	 # esp
  pushl $0x200
  pushl $0x23         	 # USER_CS
  pushl %edx      	 	 # eip
  iret
