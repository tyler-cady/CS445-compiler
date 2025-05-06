# Tyler's Dragon Assembly 
	.file	"test.p"
	.text
	.section	.rodata
.L0:
	.string "%d\n"
	.text
	.globl	main
	.type	main, @function
main:
.L1:
	pushl	%ebp
	movl	%esp, %ebp
	movl	$5, %edx

# Unimplemented procedure call: i
	movl	%edx, -4(%ebp)
	movl	-4(%ebp), %edx
	pushl	%edx
	pushl	$.L0
	call	printf
	addl	$8, %esp
	movl	$0, %eax
	popl	%ebp
	ret
.L2:
	.size	main, .-main
	.ident	"GCC: (GNU) 11.5.0 20240719 (Red Hat 11.5.0-5)"
	.section	.note.GNU-stack,"",@progbits
