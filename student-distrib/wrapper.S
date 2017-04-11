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

syscall_handler_wrapper:
  pusha
  call syscall_handler
  iret
  
user_context_switch:
  cli       
  movl 4(%esp), %edx	//Retrieve input
  movl $0x2B, %eax      // USER_DS
  movw %ax,   %ds   
  movw %ax,   %es   
  movw %ax,   %fs
  movw %ax,   %gs

  pushl $0x2B     		 // USER_DS
  pushl $0x83FFFFC  	 // esp
  pushf               	 // flags
  popl  %ecx          
  orl   $0x200,  %ecx 
  pushl %ecx         
  pushl $0x23         	 // USER_CS
  pushl %edx      	 	 // eip      
  iret                
