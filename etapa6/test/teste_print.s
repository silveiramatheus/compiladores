	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 15, 0	sdk_version 15, 2
	.globl	_main                           ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	L_.str(%rip), %rdi
	movb	$0, %al
	callq	_printf
	leaq	L_.str.1(%rip), %rdi
	movb	$0, %al
	callq	_printf
	leaq	L_.str.2(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	_numero1(%rip), %esi
	leaq	L_.str.3(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movsbl	_carac1(%rip), %esi
	leaq	L_.str.4(%rip), %rdi
	movb	$0, %al
	callq	_printf
	xorl	%eax, %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__DATA,__data
	.globl	_numero1                        ## @numero1
	.p2align	2, 0x0
_numero1:
	.long	300                             ## 0x12c

	.globl	_carac1                         ## @carac1
_carac1:
	.byte	65                              ## 0x41

	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"Ola mundo\n"

L_.str.1:                               ## @.str.1
	.asciz	"Ola mundo teste\n"

L_.str.2:                               ## @.str.2
	.asciz	"Hello World\n"

L_.str.3:                               ## @.str.3
	.asciz	"Teste numero: %d"

L_.str.4:                               ## @.str.4
	.asciz	"Caractere: %c"

.subsections_via_symbols
