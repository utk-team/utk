	.file	"sparse_matrix_sse.c"
	.text
	.align 2
.globl _Z39sparse_matrix_bcrs_double_2_2_mult_SSE2jPKdPKjS2_S0_Pd
	.type	_Z39sparse_matrix_bcrs_double_2_2_mult_SSE2jPKdPKjS2_S0_Pd, @function
_Z39sparse_matrix_bcrs_double_2_2_mult_SSE2jPKdPKjS2_S0_Pd:
.LFB3:
	pushq	%rbp
.LCFI0:
	movq	%rsp, %rbp
.LCFI1:
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	%rcx, -32(%rbp)
	movq	%r8, -40(%rbp)
	movq	%r9, -48(%rbp)
	movl	$0, %eax
	leave
	ret
.LFE3:
	.size	_Z39sparse_matrix_bcrs_double_2_2_mult_SSE2jPKdPKjS2_S0_Pd, .-_Z39sparse_matrix_bcrs_double_2_2_mult_SSE2jPKdPKjS2_S0_Pd
.globl __gxx_personality_v0
	.align 2
.globl _Z39sparse_matrix_bcrs_double_4_2_mult_SSE2jPKdPKjS2_S0_Pd
	.type	_Z39sparse_matrix_bcrs_double_4_2_mult_SSE2jPKdPKjS2_S0_Pd, @function
_Z39sparse_matrix_bcrs_double_4_2_mult_SSE2jPKdPKjS2_S0_Pd:
.LFB4:
	pushq	%rbp
.LCFI2:
	movq	%rsp, %rbp
.LCFI3:
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	%rcx, -32(%rbp)
	movq	%r8, -40(%rbp)
	movq	%r9, -48(%rbp)
	movl	$0, %eax
	leave
	ret
.LFE4:
	.size	_Z39sparse_matrix_bcrs_double_4_2_mult_SSE2jPKdPKjS2_S0_Pd, .-_Z39sparse_matrix_bcrs_double_4_2_mult_SSE2jPKdPKjS2_S0_Pd
	.align 2
.globl _Z39sparse_matrix_bcrs_double_4_4_mult_SSE2jPKdPKjS2_S0_Pd
	.type	_Z39sparse_matrix_bcrs_double_4_4_mult_SSE2jPKdPKjS2_S0_Pd, @function
_Z39sparse_matrix_bcrs_double_4_4_mult_SSE2jPKdPKjS2_S0_Pd:
.LFB5:
	pushq	%rbp
.LCFI4:
	movq	%rsp, %rbp
.LCFI5:
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	%rcx, -32(%rbp)
	movq	%r8, -40(%rbp)
	movq	%r9, -48(%rbp)
	movl	$0, %eax
	leave
	ret
.LFE5:
	.size	_Z39sparse_matrix_bcrs_double_4_4_mult_SSE2jPKdPKjS2_S0_Pd, .-_Z39sparse_matrix_bcrs_double_4_4_mult_SSE2jPKdPKjS2_S0_Pd
	.align 2
.globl _Z38sparse_matrix_bcrs_float_4_4_mult_SSE2jPKfPKjS2_S0_Pf
	.type	_Z38sparse_matrix_bcrs_float_4_4_mult_SSE2jPKfPKjS2_S0_Pf, @function
_Z38sparse_matrix_bcrs_float_4_4_mult_SSE2jPKfPKjS2_S0_Pf:
.LFB6:
	pushq	%rbp
.LCFI6:
	movq	%rsp, %rbp
.LCFI7:
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	%rcx, -32(%rbp)
	movq	%r8, -40(%rbp)
	movq	%r9, -48(%rbp)
	movl	$0, %eax
	leave
	ret
.LFE6:
	.size	_Z38sparse_matrix_bcrs_float_4_4_mult_SSE2jPKfPKjS2_S0_Pf, .-_Z38sparse_matrix_bcrs_float_4_4_mult_SSE2jPKfPKjS2_S0_Pf
	.section	.rodata
.LC0:
	.string	"sparse matrix SSE2: %d\n"
	.text
	.align 2
.globl _Z29sparse_matrix_SSE2_initializev
	.type	_Z29sparse_matrix_SSE2_initializev, @function
_Z29sparse_matrix_SSE2_initializev:
.LFB2:
	pushq	%rbp
.LCFI8:
	movq	%rsp, %rbp
.LCFI9:
	movl	SSE2_supported(%rip), %edx
	movq	stderr(%rip), %rdi
	movl	$.LC0, %esi
	movl	$0, %eax
	call	fprintf
	leave
	ret
.LFE2:
	.size	_Z29sparse_matrix_SSE2_initializev, .-_Z29sparse_matrix_SSE2_initializev
	.local	SSE2_supported
	.comm	SSE2_supported,4,4
	.section	.eh_frame,"a",@progbits
.Lframe1:
	.long	.LECIE1-.LSCIE1
.LSCIE1:
	.long	0x0
	.byte	0x1
	.string	"zPR"
	.uleb128 0x1
	.sleb128 -8
	.byte	0x10
	.uleb128 0x6
	.byte	0x3
	.long	__gxx_personality_v0
	.byte	0x3
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.byte	0x90
	.uleb128 0x1
	.align 8
.LECIE1:
.LSFDE1:
	.long	.LEFDE1-.LASFDE1
.LASFDE1:
	.long	.LASFDE1-.Lframe1
	.long	.LFB3
	.long	.LFE3-.LFB3
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI0-.LFB3
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI1-.LCFI0
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE1:
.LSFDE3:
	.long	.LEFDE3-.LASFDE3
.LASFDE3:
	.long	.LASFDE3-.Lframe1
	.long	.LFB4
	.long	.LFE4-.LFB4
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI2-.LFB4
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI3-.LCFI2
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE3:
.LSFDE5:
	.long	.LEFDE5-.LASFDE5
.LASFDE5:
	.long	.LASFDE5-.Lframe1
	.long	.LFB5
	.long	.LFE5-.LFB5
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI4-.LFB5
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI5-.LCFI4
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE5:
.LSFDE7:
	.long	.LEFDE7-.LASFDE7
.LASFDE7:
	.long	.LASFDE7-.Lframe1
	.long	.LFB6
	.long	.LFE6-.LFB6
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI6-.LFB6
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI7-.LCFI6
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE7:
.LSFDE9:
	.long	.LEFDE9-.LASFDE9
.LASFDE9:
	.long	.LASFDE9-.Lframe1
	.long	.LFB2
	.long	.LFE2-.LFB2
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI8-.LFB2
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI9-.LCFI8
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE9:
	.ident	"GCC: (GNU) 4.1.1 20070105 (Red Hat 4.1.1-51)"
	.section	.note.GNU-stack,"",@progbits
