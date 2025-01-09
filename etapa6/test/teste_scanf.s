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
	leaq	_num(%rip), %rsi
	movb	$0, %al
	callq	_scanf
	leaq	L_.str.1(%rip), %rdi
	leaq	_carac(%rip), %rsi
	movb	$0, %al
	callq	_scanf
	xorl	%eax, %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_num                            ## @num
.zerofill __DATA,__common,_num,4,2
	.globl	_carac                          ## @carac
.zerofill __DATA,__common,_carac,1,0
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"%d"

L_.str.1:                               ## @.str.1
	.asciz	"%c"

.subsections_via_symbols
