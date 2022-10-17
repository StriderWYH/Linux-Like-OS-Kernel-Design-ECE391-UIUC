.global keyboard_handler,rtc_handler 
# void KEYBOARD_handler;
# Handles keyboard_handler interrupt linkage to higher-level C function
# Inputs	: none
# Outputs	: 0 success, -1 for failure
# Registers	: Standard C calling conventions
keyboard_handler:

#    pushl	%ebp
#	pushl	%eax
#	pushl	%ebx
#	pushl	%ecx
#	pushl	%edx
	pushal
    pushfl
    call keyboard_interrupt_handler
    popfl
	popal

#	popl	%edx
#	popl	%ecx
#	popl	%ebx
#	popl	%eax
#	popl	%ebp
    iret

# void RTC_handler;
# Handles RTC interrupt linkage to higher-level C function
# Inputs	: none
# Outputs	: 0 success, -1 for failure
# Registers	: Standard C calling conventions

rtc_handler:
    pushl	%ebp
	pushl	%eax
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
    pushfl
    call rtc_interrupt_handler
    popfl
	popl	%edx
	popl	%ecx
	popl	%ebx
	popl	%eax
	popl	%ebp
    iret

# system_call_handler:
# pushal
# pushfl
# call SYSCALL
# popfl
# popal
# iret
