.text
.global systemcall_wrapper

systemcall_wrapper:
    pushl %esi
    pushl %edi
    pushl %esp

    cmpl $0,%eax
    jl invalid
    cmpl $5,%eax
    jg invalid

    pushl %edx
    pushl %ecx
    pushl %ebx
    call *systemcall_table(,%eax,4)
    popl %edx
    popl %ecx
    popl %ebx
    jmp Done

invalid:
    movl $-1,%eax
Done:
    popl %esp
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

    