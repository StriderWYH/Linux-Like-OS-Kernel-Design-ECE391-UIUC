.text
.global systemcall_wrapper

systemcall_wrapper:
    pushl %esi
    pushl %edi


    cmpl $1,%eax
    jl invalid
    cmpl $6,%eax
    jg invalid

    pushl %edx
    pushl %ecx
    pushl %ebx
    addl $-1,%eax
    call *systemcall_table(,%eax,4)
    popl %ebx
    popl %ecx
    popl %edx
    jmp Done

invalid:
    movl $-1,%eax
Done:

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

    