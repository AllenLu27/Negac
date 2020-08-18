.data
str_str_3: .asciz "The 20th term of the series is: "

str_str_2: .asciz "correct"

str_str_1: .asciz "wrong"

str_str_0: .asciz "\n"

.align 8


.text
.globl main
fun_whileloop:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $16, %rsp
movq 0(%rbp), %rax
movq %rax, -24(%rbp)
lbl_1: nop
movq -24(%rbp), %r8
movq $0, %r9
cmpq %r8, %r9
movq $0, %rcx
setne %cl
movq %rcx, -32(%rbp)
movq -32(%rbp), %rsi
cmpq $0, %rsi
je lbl_2
movq -24(%rbp), %rdi
callq printInt
movq $str_str_0, %rdi
callq printString
movq -24(%rbp), %r8
movq $1, %r9
subq %r9, %r8
movq %r8, -24(%rbp)
jmp lbl_1
lbl_2: nop
lbl_0: addq $16, %rsp
popq %rbp
retq



fun_ifelse:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $8, %rsp
movq $0, %rax
movq %rax, -24(%rbp)
movq -24(%rbp), %rsi
cmpq $0, %rsi
je lbl_4
movq $str_str_1, %rdi
callq printString
movq $str_str_0, %rdi
callq printString
jmp lbl_5
lbl_4: nop
movq $str_str_2, %rdi
callq printString
movq $str_str_0, %rdi
callq printString
lbl_5: nop
lbl_3: addq $8, %rsp
popq %rbp
retq



fun_binaryOp:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $48, %rsp
movq $3, %rax
movq %rax, -24(%rbp)
movq $10, %rax
movq %rax, -32(%rbp)
movq $2, %rax
movq %rax, -40(%rbp)
movq -32(%rbp), %r8
movq -24(%rbp), %r9
subq %r9, %r8
movq %r8, -48(%rbp)
movq -48(%rbp), %rdi
callq printInt
movq $str_str_0, %rdi
callq printString
movq -32(%rbp), %rax
movq -24(%rbp), %r8
imulq %r8
movq %rax, -56(%rbp)
movq -56(%rbp), %rdi
callq printInt
movq $str_str_0, %rdi
callq printString
movq $0, %rdx
movq -32(%rbp), %rax
movq -40(%rbp), %r8
idivq %r8
movq %rax, -64(%rbp)
movq -64(%rbp), %rdi
callq printInt
movq $str_str_0, %rdi
callq printString
lbl_6: addq $48, %rsp
popq %rbp
retq



fun_binaryOp1:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $56, %rsp
movq $3, %r8
movq $5, %r9
cmpq %r9, %r8
movq $0, %rcx
setl %cl
movq %rcx, -40(%rbp)
movq -40(%rbp), %rax
movq %rax, -24(%rbp)
movq -24(%rbp), %rdi
callq printInt
movq $str_str_0, %rdi
callq printString
movq $7, %r8
movq $5, %r9
cmpq %r9, %r8
movq $0, %rcx
setle %cl
movq %rcx, -48(%rbp)
movq -48(%rbp), %rax
movq %rax, -24(%rbp)
movq -24(%rbp), %rdi
callq printInt
movq $str_str_0, %rdi
callq printString
movq $0, %r8
movq $0, %r9
andq %r8, %r9
movq %r9, -56(%rbp)
movq -56(%rbp), %rax
movq %rax, -32(%rbp)
movq -32(%rbp), %rdi
callq printInt
movq $str_str_0, %rdi
callq printString
movq $3, %r8
movq $5, %r9
cmpq %r9, %r8
movq $0, %rcx
setl %cl
movq %rcx, -64(%rbp)
movq -64(%rbp), %r8
movq $0, %r9
orq %r8, %r9
movq %r9, -72(%rbp)
movq -72(%rbp), %rax
movq %rax, -32(%rbp)
movq -32(%rbp), %rdi
callq printInt
movq $str_str_0, %rdi
callq printString
lbl_7: addq $56, %rsp
popq %rbp
retq



fun_unaryOp:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $40, %rsp
movq $0, %rdx
movq $15, %rax
subq %rax, %rdx
movq %rdx, -40(%rbp)
movq -40(%rbp), %rax
movq %rax, -24(%rbp)
movq $7, %r8
movq $10, %r9
cmpq %r9, %r8
movq $0, %rcx
setle %cl
movq %rcx, -48(%rbp)
movq -48(%rbp), %rax
movq %rax, -32(%rbp)
movq -24(%rbp), %rdi
callq printInt
movq $str_str_0, %rdi
callq printString
movq $1, %rdx
movq -32(%rbp), %rax
xorq %rdx, %rax
movq %rax, -56(%rbp)
movq -56(%rbp), %rax
movq %rax, -32(%rbp)
movq -32(%rbp), %rdi
callq printInt
movq $str_str_0, %rdi
callq printString
lbl_8: addq $40, %rsp
popq %rbp
retq



fun_fib:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $64, %rsp
movq 0(%rbp), %rax
movq %rax, -24(%rbp)
movq -24(%rbp), %r8
movq $1, %r9
cmpq %r9, %r8
movq $0, %rcx
setle %cl
movq %rcx, -32(%rbp)
movq -32(%rbp), %rsi
cmpq $0, %rsi
je lbl_10
movq -24(%rbp), %rax
jmp lbl_9
lbl_10: nop
movq -24(%rbp), %r8
movq $1, %r9
subq %r9, %r8
movq %r8, -40(%rbp)
movq -40(%rbp), %rdx
pushq %rdx
callq fun_fib
addq $8, %rsp
movq %rax, -48(%rbp)
movq -24(%rbp), %r8
movq $2, %r9
subq %r9, %r8
movq %r8, -56(%rbp)
movq -56(%rbp), %rdx
pushq %rdx
callq fun_fib
addq $8, %rsp
movq %rax, -64(%rbp)
movq $0, %rdx
movq -64(%rbp), %rax
subq %rax, %rdx
movq %rdx, -72(%rbp)
movq -48(%rbp), %r8
movq -72(%rbp), %r9
subq %r9, %r8
movq %r8, -80(%rbp)
movq -80(%rbp), %rax
jmp lbl_9
lbl_9: addq $64, %rsp
popq %rbp
retq



main:
pushq %rbp
movq %rsp, %rbp
addq $16, %rbp
subq $8, %rsp
callq fun_ifelse
addq $0, %rsp
movq $5, %rdx
pushq %rdx
callq fun_whileloop
addq $8, %rsp
callq fun_binaryOp
addq $0, %rsp
callq fun_binaryOp1
addq $0, %rsp
callq fun_unaryOp
addq $0, %rsp
movq $str_str_3, %rdi
callq printString
movq $20, %rdx
pushq %rdx
callq fun_fib
addq $8, %rsp
movq %rax, -24(%rbp)
movq -24(%rbp), %rdi
callq printInt
movq $str_str_0, %rdi
callq printString
lbl_11: addq $8, %rsp
movq $60, %rax
movq $0, %rdi
syscall



