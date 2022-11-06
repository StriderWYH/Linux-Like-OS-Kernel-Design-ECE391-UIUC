.text
.global systemcall_wrapper

#system call wrapper is a function used to call all the systemcalls in kernel
#input: eax
systemcall_wrapper:
    pushl %esi
    pushl %edi
    pushl %ebp
    pushl %esp  #callee saved register

    pushfl      #push all flags

    cmpl $1,%eax    #check if the eax number is valid or not
    jl invalid
    cmpl $7,%eax
    jg invalid

    pushl %edx      
    pushl %ecx
    pushl %ebx      #push parameters
    addl $-1,%eax   #eax pushed into the systemcall_table should start at 0
    call *systemcall_table(,%eax,4)
    popl %ebx       #pop paramaters
    popl %ecx
    popl %edx
    jmp Done

invalid:
    movl $-1,%eax
Done:

    popfl       #pop all flags
    popl %esp   #pop all callee_saved registers
    popl %ebp
    popl %edi
    popl %esi

    IRET

        

systemcall_table:
    .long	halt
    .long   execute
    .long	read
    .long	write
    .long	open
    .long   close
    .long   getargs

    