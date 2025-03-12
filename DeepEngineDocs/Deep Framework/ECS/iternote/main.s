	.file	"main.cpp"
	.text
	.p2align 4
	.globl	_Z5Loop1RSt6vectorI9ComponentSaIS0_EE
	.def	_Z5Loop1RSt6vectorI9ComponentSaIS0_EE;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z5Loop1RSt6vectorI9ComponentSaIS0_EE
_Z5Loop1RSt6vectorI9ComponentSaIS0_EE:
.LFB9574:
	.seh_endprologue
	movq	8(%rcx), %rax
	movq	(%rcx), %r8
	movq	%rax, %rcx
	subq	%r8, %rcx
	movq	%rcx, %r9
	sarq	$2, %r9
	cmpq	%r8, %rax
	je	.L1
	testq	%rcx, %rcx
	movl	$1, %eax
	cmovne	%r9, %rax
	movq	%rax, %rdx
	cmpq	$12, %rcx
	jbe	.L6
	movq	%rdx, %rcx
	movdqu	.LC0(%rip), %xmm1
	movq	%r8, %rax
	movdqu	.LC1(%rip), %xmm4
	shrq	$2, %rcx
	movdqu	.LC2(%rip), %xmm3
	salq	$4, %rcx
	addq	%r8, %rcx
	.p2align 4,,10
	.p2align 3
.L4:
	movdqa	%xmm1, %xmm0
	addq	$16, %rax
	paddq	%xmm4, %xmm1
	movdqa	%xmm0, %xmm2
	paddq	%xmm3, %xmm2
	shufps	$136, %xmm2, %xmm0
	movups	%xmm0, -16(%rax)
	cmpq	%rcx, %rax
	jne	.L4
	testb	$3, %dl
	je	.L1
	movq	%rdx, %rax
	andq	$-4, %rax
.L3:
	leaq	1(%rax), %rdx
	movl	%eax, (%r8,%rax,4)
	leaq	0(,%rax,4), %rcx
	cmpq	%r9, %rdx
	jnb	.L1
	addq	$2, %rax
	movl	%edx, 4(%r8,%rcx)
	cmpq	%r9, %rax
	jnb	.L1
	movl	%eax, 8(%r8,%rcx)
.L1:
	ret
.L6:
	xorl	%eax, %eax
	jmp	.L3
	.seh_endproc
	.p2align 4
	.globl	_Z5Loop2RN4Deep4ECDB9ArchetypeEi
	.def	_Z5Loop2RN4Deep4ECDB9ArchetypeEi;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z5Loop2RN4Deep4ECDB9ArchetypeEi
_Z5Loop2RN4Deep4ECDB9ArchetypeEi:
.LFB9575:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, %rdi
	movl	%edx, %r8d
	leaq	32(%rsp), %rcx
	movq	%rdi, %rdx
	call	_ZNK4Deep4ECDB9Archetype18GetComponentOffsetEi
	movq	104(%rdi), %r10
	movq	32(%rsp), %rsi
	testq	%r10, %r10
	je	.L13
	movq	80(%rdi), %rbx
	movq	88(%rdi), %r11
	movq	%r10, %r8
	movdqu	.LC1(%rip), %xmm4
	movdqu	.LC2(%rip), %xmm3
	.p2align 4,,10
	.p2align 3
.L18:
	cmpq	%r8, %r10
	movq	%r11, %rcx
	cmovne	%rbx, %rcx
	testq	%rcx, %rcx
	je	.L14
	leaq	-1(%rcx), %rax
	cmpq	$2, %rax
	jbe	.L19
	movq	%rcx, %rdx
	leaq	(%r8,%rsi), %r9
	movdqu	.LC0(%rip), %xmm1
	shrq	$2, %rdx
	movq	%r9, %rax
	salq	$4, %rdx
	addq	%r9, %rdx
	.p2align 4,,10
	.p2align 3
.L16:
	movdqa	%xmm1, %xmm0
	addq	$16, %rax
	paddq	%xmm4, %xmm1
	movdqa	%xmm0, %xmm2
	paddq	%xmm3, %xmm2
	shufps	$136, %xmm2, %xmm0
	movups	%xmm0, -16(%rax)
	cmpq	%rax, %rdx
	jne	.L16
	testb	$3, %cl
	je	.L14
	movq	%rcx, %rax
	andq	$-4, %rax
.L15:
	leaq	(%r9,%rax,4), %rdx
	leaq	1(%rax), %r9
	movl	%eax, (%rdx)
	cmpq	%rcx, %r9
	jnb	.L14
	addq	$2, %rax
	movl	%r9d, 4(%rdx)
	cmpq	%rcx, %rax
	jnb	.L14
	movl	%eax, 8(%rdx)
.L14:
	movq	16376(%r8), %r8
	testq	%r8, %r8
	jne	.L18
.L13:
	addq	$48, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	ret
.L19:
	xorl	%eax, %eax
	leaq	(%r8,%rsi), %r9
	jmp	.L15
	.seh_endproc
	.section	.text$_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEED1Ev,"x"
	.linkonce discard
	.align 2
	.p2align 4
	.globl	_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEED1Ev
	.def	_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEED1Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEED1Ev
_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEED1Ev:
.LFB9882:
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	16(%rcx), %rbx
	movq	%rcx, %rdi
	testq	%rbx, %rbx
	jne	.L35
	jmp	.L32
	.p2align 4,,10
	.p2align 3
.L43:
	movq	%rsi, %rbx
.L35:
	movq	16(%rbx), %rcx
	movq	(%rbx), %rsi
	testq	%rcx, %rcx
	je	.L33
	movq	32(%rbx), %rdx
	subq	%rcx, %rdx
	call	_ZdlPvy
.L33:
	movl	$56, %edx
	movq	%rbx, %rcx
	call	_ZdlPvy
	testq	%rsi, %rsi
	jne	.L43
.L32:
	movq	8(%rdi), %rax
	movq	(%rdi), %rcx
	xorl	%edx, %edx
	addq	$48, %rdi
	leaq	0(,%rax,8), %r8
	call	memset
	movq	-48(%rdi), %rcx
	movq	-40(%rdi), %rdx
	movq	$0, -24(%rdi)
	movq	$0, -32(%rdi)
	cmpq	%rdi, %rcx
	je	.L31
	salq	$3, %rdx
	addq	$32, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	jmp	_ZdlPvy
	.p2align 4,,10
	.p2align 3
.L31:
	addq	$32, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	ret
	.seh_endproc
	.section	.text$_ZNSt6vectorIN4Deep13ComponentDescESaIS1_EED1Ev,"x"
	.linkonce discard
	.align 2
	.p2align 4
	.globl	_ZNSt6vectorIN4Deep13ComponentDescESaIS1_EED1Ev
	.def	_ZNSt6vectorIN4Deep13ComponentDescESaIS1_EED1Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZNSt6vectorIN4Deep13ComponentDescESaIS1_EED1Ev
_ZNSt6vectorIN4Deep13ComponentDescESaIS1_EED1Ev:
.LFB9907:
	.seh_endprologue
	movq	(%rcx), %rax
	testq	%rax, %rax
	je	.L44
	movq	16(%rcx), %rdx
	movq	%rax, %rcx
	subq	%rax, %rdx
	jmp	_ZdlPvy
	.p2align 4,,10
	.p2align 3
.L44:
	ret
	.seh_endproc
	.section	.text$_ZNSt6vectorI9ComponentSaIS0_EED1Ev,"x"
	.linkonce discard
	.align 2
	.p2align 4
	.globl	_ZNSt6vectorI9ComponentSaIS0_EED1Ev
	.def	_ZNSt6vectorI9ComponentSaIS0_EED1Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZNSt6vectorI9ComponentSaIS0_EED1Ev
_ZNSt6vectorI9ComponentSaIS0_EED1Ev:
.LFB9965:
	.seh_endprologue
	movq	(%rcx), %rax
	testq	%rax, %rax
	je	.L46
	movq	16(%rcx), %rdx
	movq	%rax, %rcx
	subq	%rax, %rdx
	jmp	_ZdlPvy
	.p2align 4,,10
	.p2align 3
.L46:
	ret
	.seh_endproc
	.section	.text$_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEE12_Scoped_nodeD1Ev,"x"
	.linkonce discard
	.align 2
	.p2align 4
	.globl	_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEE12_Scoped_nodeD1Ev
	.def	_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEE12_Scoped_nodeD1Ev;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEE12_Scoped_nodeD1Ev
_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEE12_Scoped_nodeD1Ev:
.LFB10368:
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	8(%rcx), %rbx
	testq	%rbx, %rbx
	je	.L48
	movq	16(%rbx), %rcx
	testq	%rcx, %rcx
	je	.L50
	movq	32(%rbx), %rdx
	subq	%rcx, %rdx
	call	_ZdlPvy
.L50:
	movl	$56, %edx
	movq	%rbx, %rcx
	addq	$32, %rsp
	popq	%rbx
	jmp	_ZdlPvy
	.p2align 4,,10
	.p2align 3
.L48:
	addq	$32, %rsp
	popq	%rbx
	ret
	.seh_endproc
	.section	.text$_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEE9_M_rehashEyRKy,"x"
	.linkonce discard
	.align 2
	.p2align 4
	.globl	_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEE9_M_rehashEyRKy
	.def	_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEE9_M_rehashEyRKy;	.scl	2;	.type	32;	.endef
	.seh_proc	_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEE9_M_rehashEyRKy
_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEE9_M_rehashEyRKy:
.LFB10477:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$40, %rsp
	.seh_stackalloc	40
	.seh_endprologue
	movq	%rcx, %rsi
	movq	%rdx, %rbx
	movq	%r8, %rdi
	cmpq	$1, %rdx
	je	.L74
	movq	%rdx, %rax
	shrq	$60, %rax
	jne	.L75
	leaq	0(,%rdx,8), %rbp
	movq	%rbp, %rcx
.LEHB0:
	call	_Znwy
	movq	%rbp, %r8
	xorl	%edx, %edx
	movq	%rax, %rcx
	movq	%rax, %rdi
	call	memset
	leaq	48(%rsi), %rbp
.L56:
	movq	16(%rsi), %r8
	movq	$0, 16(%rsi)
	testq	%r8, %r8
	je	.L59
	xorl	%r10d, %r10d
	leaq	16(%rsi), %r11
	jmp	.L63
	.p2align 4,,10
	.p2align 3
.L60:
	movq	(%r9), %rdx
	movq	%rdx, (%rcx)
	movq	(%rax), %rax
	movq	%rcx, (%rax)
	testq	%r8, %r8
	je	.L59
.L63:
	movq	%r8, %rcx
	xorl	%edx, %edx
	movq	(%r8), %r8
	movq	48(%rcx), %rax
	divq	%rbx
	leaq	(%rdi,%rdx,8), %rax
	movq	(%rax), %r9
	testq	%r9, %r9
	jne	.L60
	movq	16(%rsi), %r9
	movq	%r9, (%rcx)
	movq	%rcx, 16(%rsi)
	movq	%r11, (%rax)
	cmpq	$0, (%rcx)
	je	.L61
	movq	%rcx, (%rdi,%r10,8)
.L61:
	movq	%rdx, %r10
	testq	%r8, %r8
	jne	.L63
.L59:
	movq	(%rsi), %rcx
	movq	8(%rsi), %rdx
	cmpq	%rbp, %rcx
	je	.L64
	salq	$3, %rdx
	call	_ZdlPvy
.L64:
	movq	%rbx, 8(%rsi)
	movq	%rdi, (%rsi)
	addq	$40, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret
	.p2align 4,,10
	.p2align 3
.L74:
	leaq	48(%rcx), %rbp
	movq	$0, 48(%rcx)
	movq	%rbp, %rdi
	jmp	.L56
	.p2align 4,,10
	.p2align 3
.L75:
	shrq	$61, %rbx
	je	.L58
	call	_ZSt28__throw_bad_array_new_lengthv
.L58:
	call	_ZSt17__throw_bad_allocv
.LEHE0:
.L67:
	movq	%rax, %rcx
	call	__cxa_begin_catch
	movq	(%rdi), %rax
	movq	%rax, 40(%rsi)
.LEHB1:
	call	__cxa_rethrow
.LEHE1:
.L68:
	movq	%rax, %rbx
	call	__cxa_end_catch
	movq	%rbx, %rcx
.LEHB2:
	call	_Unwind_Resume
	nop
.LEHE2:
	.def	__gxx_personality_seh0;	.scl	2;	.type	32;	.endef
	.seh_handler	__gxx_personality_seh0, @unwind, @except
	.seh_handlerdata
	.align 4
.LLSDA10477:
	.byte	0xff
	.byte	0x9b
	.uleb128 .LLSDATT10477-.LLSDATTD10477
.LLSDATTD10477:
	.byte	0x1
	.uleb128 .LLSDACSE10477-.LLSDACSB10477
.LLSDACSB10477:
	.uleb128 .LEHB0-.LFB10477
	.uleb128 .LEHE0-.LEHB0
	.uleb128 .L67-.LFB10477
	.uleb128 0x1
	.uleb128 .LEHB1-.LFB10477
	.uleb128 .LEHE1-.LEHB1
	.uleb128 .L68-.LFB10477
	.uleb128 0
	.uleb128 .LEHB2-.LFB10477
	.uleb128 .LEHE2-.LEHB2
	.uleb128 0
	.uleb128 0
.LLSDACSE10477:
	.byte	0x1
	.byte	0
	.align 4
	.long	0

.LLSDATT10477:
	.section	.text$_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEE9_M_rehashEyRKy,"x"
	.linkonce discard
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.section	.text.startup,"x"
	.p2align 4
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
.LFB9576:
	pushq	%r15
	.seh_pushreg	%r15
	pushq	%r14
	.seh_pushreg	%r14
	pushq	%r13
	.seh_pushreg	%r13
	pushq	%r12
	.seh_pushreg	%r12
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$296, %rsp
	.seh_stackalloc	296
	movups	%xmm6, 272(%rsp)
	.seh_savexmm	%xmm6, 272
	.seh_endprologue
	call	__main
	leaq	112(%rsp), %rax
	pxor	%xmm0, %xmm0
	movss	.LC3(%rip), %xmm6
	movq	%rax, %rdi
	movq	%rax, 56(%rsp)
	leaq	232(%rsp), %rax
	movl	$272, %ecx
	movq	%rdi, %xmm1
	movq	%rax, %xmm5
	movq	$0, 128(%rsp)
	leaq	184(%rsp), %r13
	punpcklqdq	%xmm5, %xmm1
	movq	$1, 192(%rsp)
	movq	$0, 200(%rsp)
	movq	$0, 208(%rsp)
	movq	$0, 224(%rsp)
	movq	$0, 232(%rsp)
	movq	$0, 248(%rsp)
	movups	%xmm0, 112(%rsp)
	movups	%xmm1, 176(%rsp)
	movss	%xmm6, 216(%rsp)
	movups	%xmm0, 256(%rsp)
.LEHB3:
	call	_Znwy
	movdqu	.LC4(%rip), %xmm1
	leaq	176(%rsp), %r12
	movq	%rax, %rbx
	movl	$56, %ecx
	movddup	176(%rsp), %xmm0
	movups	%xmm0, (%rax)
	pxor	%xmm0, %xmm0
	leaq	208(%rax), %rax
	movups	%xmm0, -192(%rax)
	movups	%xmm0, -176(%rax)
	movups	%xmm0, -160(%rax)
	movq	%r12, -144(%rax)
	movups	%xmm1, -136(%rax)
	movq	$0, -120(%rax)
	movups	%xmm0, -112(%rax)
	movups	%xmm0, -96(%rax)
	movups	%xmm0, -80(%rax)
	movups	%xmm0, -64(%rax)
	movq	%rax, 160(%rbx)
	leaq	264(%rbx), %rax
	movq	$1, 168(%rbx)
	movq	$0, 176(%rbx)
	movq	$0, 184(%rbx)
	movq	$0, 200(%rbx)
	movq	$0, 208(%rbx)
	movq	%rax, 216(%rbx)
	movq	$1, 224(%rbx)
	movq	$0, 232(%rbx)
	movq	$0, 240(%rbx)
	movq	$0, 256(%rbx)
	movq	$0, 264(%rbx)
	movss	%xmm6, 192(%rbx)
	movss	%xmm6, 248(%rbx)
	movq	%rbx, 240(%rsp)
	movq	%r13, 144(%rsp)
	call	_Znwy
.LEHE3:
	movq	$0, (%rax)
	movq	%rax, %rsi
	movq	8(%rbx), %rax
	movq	$0, 16(%rsi)
	movq	%rax, 8(%rsi)
	movq	24(%rbx), %rax
	subq	16(%rbx), %rax
	movq	$0, 24(%rsi)
	movq	$0, 32(%rsi)
	movq	%rax, %rdi
	je	.L121
	movabsq	$9223372036854775804, %rax
	cmpq	%rdi, %rax
	jb	.L150
	movq	%rdi, %rcx
.LEHB4:
	call	_Znwy
.LEHE4:
	movq	%rax, %rcx
.L77:
	movq	16(%rbx), %rdx
	movq	24(%rbx), %rbx
	movq	%rcx, %xmm5
	leaq	(%rcx,%rdi), %rax
	movddup	%xmm5, %xmm0
	movq	%rax, 32(%rsi)
	subq	%rdx, %rbx
	movups	%xmm0, 16(%rsi)
	cmpq	$4, %rbx
	jle	.L79
	movq	%rbx, %r8
	call	memmove
	movq	%rax, %rcx
.L80:
	addq	%rbx, %rcx
	movq	240(%rsp), %rax
	movq	208(%rsp), %rbx
	leaq	8(%rsi), %rbp
	movq	%rcx, 24(%rsi)
	movq	%rax, 40(%rsi)
	movq	%rbx, %r9
	movq	%rsi, 152(%rsp)
	testq	%rbx, %rbx
	jne	.L151
	movq	200(%rsp), %rdi
	testq	%rdi, %rdi
	jne	.L91
	jmp	.L88
	.p2align 4,,10
	.p2align 3
.L153:
	movq	(%rdi), %rdi
	testq	%rdi, %rdi
	je	.L152
.L91:
	leaq	8(%rdi), %rdx
	movq	%rbp, %rcx
.LEHB5:
	call	_ZN4DeepeqERKNS_17ArchetypeBitFieldES2_
.LEHE5:
	testb	%al, %al
	je	.L153
.L90:
	leaq	144(%rsp), %r15
	movq	%r15, %rcx
	call	_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEE12_Scoped_nodeD1Ev
	movq	56(%rsp), %rcx
	xorl	%r9d, %r9d
	movl	$4, %r8d
	movl	$4, %edx
.LEHB6:
	call	_ZN4Deep10ECRegistry17RegisterComponentEyyPKc
	leaq	104(%rsp), %rdx
	movq	%r12, %rcx
	movl	%eax, 104(%rsp)
	movl	%eax, %r13d
	movl	$1, %r8d
	call	_ZN4Deep4ECDB12GetArchetypeEPiy
	movq	%rax, %rbx
	movl	$10000, %esi
	leaq	64(%rsp), %rdi
	.p2align 4,,10
	.p2align 3
.L112:
	movq	%rbx, %rdx
	movq	%rdi, %rcx
	call	_ZN4Deep4ECDB9Archetype6EntityEv
	subq	$1, %rsi
	jne	.L112
	pxor	%xmm0, %xmm0
	movl	$40000, %ecx
	movups	%xmm0, 152(%rsp)
	call	_Znwy
.LEHE6:
	leaq	40000(%rax), %rdi
	movl	$0, (%rax)
	movq	%rax, %rsi
	leaq	4(%rax), %rdx
	movq	%rax, 144(%rsp)
	movq	%rdi, 160(%rsp)
	.p2align 4,,10
	.p2align 3
.L113:
	movl	(%rsi), %eax
	addq	$4, %rdx
	movl	%eax, -4(%rdx)
	cmpq	%rdx, %rdi
	jne	.L113
	movq	%rdi, 152(%rsp)
	movdqu	.LC0(%rip), %xmm1
	movq	%rsi, %rax
	movdqu	.LC1(%rip), %xmm4
	movdqu	.LC2(%rip), %xmm3
	.p2align 4,,10
	.p2align 3
.L114:
	movdqa	%xmm1, %xmm0
	addq	$16, %rax
	paddq	%xmm4, %xmm1
	movdqa	%xmm0, %xmm2
	paddq	%xmm3, %xmm2
	shufps	$136, %xmm2, %xmm0
	movups	%xmm0, -16(%rax)
	cmpq	%rax, %rdi
	jne	.L114
	movq	.refptr._ZSt4cout(%rip), %rbp
	leaq	103(%rsp), %r14
	jmp	.L117
	.p2align 4,,10
	.p2align 3
.L155:
	movl	$1, %r8d
	movq	%r14, %rdx
.LEHB7:
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_x
	addq	$4, %rsi
	cmpq	%rsi, %rdi
	je	.L154
.L117:
	movl	(%rsi), %edx
	movq	%rbp, %rcx
	call	_ZNSolsEi
	movq	%rax, %rcx
	movq	(%rax), %rax
	movb	$10, 103(%rsp)
	movq	-24(%rax), %rax
	cmpq	$0, 16(%rcx,%rax)
	jne	.L155
	movl	$10, %edx
	call	_ZNSo3putEc
	addq	$4, %rsi
	cmpq	%rsi, %rdi
	jne	.L117
.L154:
	movl	%r13d, %edx
	movq	%rbx, %rcx
	call	_Z5Loop2RN4Deep4ECDB9ArchetypeEi
.LEHE7:
	movq	%r15, %rcx
	call	_ZNSt6vectorI9ComponentSaIS0_EED1Ev
	movq	%r12, %rcx
	call	_ZN4Deep4ECDBD1Ev
	movq	56(%rsp), %rcx
	call	_ZNSt6vectorIN4Deep13ComponentDescESaIS1_EED1Ev
	nop
	movups	272(%rsp), %xmm6
	xorl	%eax, %eax
	addq	$296, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	ret
.L151:
	movq	16(%rsi), %r8
	movq	%rcx, %r10
	xorl	%ebx, %ebx
	xorl	%r14d, %r14d
	subq	%r8, %r10
	sarq	$2, %r10
	cmpq	%rcx, %r8
	je	.L95
.L83:
	xorl	%ecx, %ecx
	xorl	%ebx, %ebx
	xorl	%eax, %eax
	movl	$2884619999, %r9d
	.p2align 4,,10
	.p2align 3
.L94:
	movl	(%r8,%rax,4), %edx
	testl	%edx, %edx
	je	.L93
	xorq	%rcx, %rbx
	xorq	%rdx, %rbx
.L93:
	addq	$1, %rax
	addq	%r9, %rcx
	cmpq	%r10, %rax
	jb	.L94
.L84:
	xorl	%edx, %edx
	movq	%rbx, %rax
	divq	192(%rsp)
	cmpq	$0, 208(%rsp)
	movq	%rdx, %r14
	jne	.L95
	movq	%rbx, %rdi
	xorl	%r9d, %r9d
	movq	%rdx, %rbx
.L98:
	movq	$1, 32(%rsp)
	movq	224(%rsp), %rax
	leaq	80(%rsp), %rcx
	leaq	216(%rsp), %rdx
	movq	192(%rsp), %r8
	movq	%rax, 104(%rsp)
.LEHB8:
	call	_ZNKSt8__detail20_Prime_rehash_policy14_M_need_rehashEyyy
	cmpb	$0, 80(%rsp)
	movq	88(%rsp), %rdx
	jne	.L156
.L105:
	movq	184(%rsp), %r8
	movq	%rdi, 48(%rsi)
	leaq	(%r8,%rbx,8), %rcx
	movq	(%rcx), %rax
	testq	%rax, %rax
	je	.L106
	movq	(%rax), %rax
	movq	%rax, (%rsi)
	movq	(%rcx), %rax
	movq	%rsi, (%rax)
.L107:
	addq	$1, 208(%rsp)
	xorl	%eax, %eax
	movq	%rax, 152(%rsp)
	jmp	.L90
.L121:
	xorl	%ecx, %ecx
	jmp	.L77
.L95:
	movq	184(%rsp), %rax
	movq	(%rax,%r14,8), %r15
	testq	%r15, %r15
	je	.L100
	movq	(%r15), %rdi
	movq	48(%rdi), %rcx
	jmp	.L104
	.p2align 4,,10
	.p2align 3
.L103:
	movq	(%rdi), %r8
	testq	%r8, %r8
	je	.L100
	movq	48(%r8), %rcx
	xorl	%edx, %edx
	movq	%rdi, %r15
	movq	%rcx, %rax
	divq	192(%rsp)
	cmpq	%r14, %rdx
	jne	.L100
	movq	%r8, %rdi
.L104:
	cmpq	%rcx, %rbx
	jne	.L103
	leaq	8(%rdi), %rdx
	movq	%rbp, %rcx
	call	_ZN4DeepeqERKNS_17ArchetypeBitFieldES2_
	testb	%al, %al
	je	.L103
	cmpq	$0, (%r15)
	jne	.L90
.L100:
	movq	%rbx, %rdi
	movq	208(%rsp), %r9
	movq	%r14, %rbx
	jmp	.L98
.L152:
	movq	24(%rsi), %rax
	movq	16(%rsi), %r8
	movq	%rax, %r10
	subq	%r8, %r10
	sarq	$2, %r10
	cmpq	%r8, %rax
	jne	.L83
	jmp	.L84
.L156:
	leaq	104(%rsp), %r8
	movq	%r13, %rcx
	call	_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEE9_M_rehashEyRKy
.LEHE8:
	movq	%rdi, %rax
	xorl	%edx, %edx
	divq	192(%rsp)
	movq	%rdx, %rbx
	jmp	.L105
.L106:
	movq	200(%rsp), %rax
	movq	%rsi, 200(%rsp)
	movq	%rax, (%rsi)
	testq	%rax, %rax
	je	.L108
	movq	48(%rax), %rax
	xorl	%edx, %edx
	divq	192(%rsp)
	movq	%rsi, (%r8,%rdx,8)
.L108:
	leaq	200(%rsp), %rax
	movq	%rax, (%rcx)
	jmp	.L107
.L79:
	jne	.L80
	movl	(%rdx), %eax
	movl	%eax, (%rcx)
	jmp	.L80
.L150:
.LEHB9:
	call	_ZSt28__throw_bad_array_new_lengthv
.LEHE9:
.L88:
	movq	16(%rsi), %r8
	movq	%rcx, %r10
	xorl	%edi, %edi
	subq	%r8, %r10
	sarq	$2, %r10
	cmpq	%rcx, %r8
	jne	.L83
	jmp	.L98
.L125:
	leaq	144(%rsp), %rcx
	movq	%rax, %rbx
	call	_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEE12_Scoped_nodeD1Ev
.L87:
	movq	%r13, %rcx
	call	_ZNSt10_HashtableIN4Deep17ArchetypeBitFieldESt4pairIKS1_PNS0_4ECDB9ArchetypeEESaIS7_ENSt8__detail10_Select1stESt8equal_toIS1_ESt4hashIS1_ENS9_18_Mod_range_hashingENS9_20_Default_ranged_hashENS9_20_Prime_rehash_policyENS9_17_Hashtable_traitsILb1ELb0ELb1EEEED1Ev
.L111:
	movq	56(%rsp), %rcx
	call	_ZNSt6vectorIN4Deep13ComponentDescESaIS1_EED1Ev
	movq	%rbx, %rcx
.LEHB10:
	call	_Unwind_Resume
.LEHE10:
.L124:
	movq	%rax, %rbx
	jmp	.L87
.L126:
	movq	%rax, %rcx
	call	__cxa_begin_catch
	movl	$56, %edx
	movq	%rsi, %rcx
	call	_ZdlPvy
.LEHB11:
	call	__cxa_rethrow
.LEHE11:
.L122:
	movq	%rax, %rbx
	jmp	.L119
.L123:
	movq	%r15, %rcx
	movq	%rax, %rbx
	call	_ZNSt6vectorI9ComponentSaIS0_EED1Ev
.L119:
	movq	%r12, %rcx
	call	_ZN4Deep4ECDBD1Ev
	jmp	.L111
.L127:
	movq	%rax, %rbx
	call	__cxa_end_catch
	jmp	.L87
	.seh_handler	__gxx_personality_seh0, @unwind, @except
	.seh_handlerdata
	.align 4
.LLSDA9576:
	.byte	0xff
	.byte	0x9b
	.uleb128 .LLSDATT9576-.LLSDATTD9576
.LLSDATTD9576:
	.byte	0x1
	.uleb128 .LLSDACSE9576-.LLSDACSB9576
.LLSDACSB9576:
	.uleb128 .LEHB3-.LFB9576
	.uleb128 .LEHE3-.LEHB3
	.uleb128 .L124-.LFB9576
	.uleb128 0
	.uleb128 .LEHB4-.LFB9576
	.uleb128 .LEHE4-.LEHB4
	.uleb128 .L126-.LFB9576
	.uleb128 0x1
	.uleb128 .LEHB5-.LFB9576
	.uleb128 .LEHE5-.LEHB5
	.uleb128 .L125-.LFB9576
	.uleb128 0
	.uleb128 .LEHB6-.LFB9576
	.uleb128 .LEHE6-.LEHB6
	.uleb128 .L122-.LFB9576
	.uleb128 0
	.uleb128 .LEHB7-.LFB9576
	.uleb128 .LEHE7-.LEHB7
	.uleb128 .L123-.LFB9576
	.uleb128 0
	.uleb128 .LEHB8-.LFB9576
	.uleb128 .LEHE8-.LEHB8
	.uleb128 .L125-.LFB9576
	.uleb128 0
	.uleb128 .LEHB9-.LFB9576
	.uleb128 .LEHE9-.LEHB9
	.uleb128 .L126-.LFB9576
	.uleb128 0x1
	.uleb128 .LEHB10-.LFB9576
	.uleb128 .LEHE10-.LEHB10
	.uleb128 0
	.uleb128 0
	.uleb128 .LEHB11-.LFB9576
	.uleb128 .LEHE11-.LEHB11
	.uleb128 .L127-.LFB9576
	.uleb128 0
.LLSDACSE9576:
	.byte	0x1
	.byte	0
	.align 4
	.long	0

.LLSDATT9576:
	.section	.text.startup,"x"
	.seh_endproc
	.section .rdata,"dr"
	.align 16
.LC0:
	.quad	0
	.quad	1
	.align 16
.LC1:
	.quad	4
	.quad	4
	.align 16
.LC2:
	.quad	2
	.quad	2
	.align 4
.LC3:
	.long	1065353216
	.align 16
.LC4:
	.quad	0
	.quad	2047
	.ident	"GCC: (Rev7, Built by MSYS2 project) 13.1.0"
	.def	_ZNK4Deep4ECDB9Archetype18GetComponentOffsetEi;	.scl	2;	.type	32;	.endef
	.def	_ZdlPvy;	.scl	2;	.type	32;	.endef
	.def	memset;	.scl	2;	.type	32;	.endef
	.def	_Znwy;	.scl	2;	.type	32;	.endef
	.def	_ZSt28__throw_bad_array_new_lengthv;	.scl	2;	.type	32;	.endef
	.def	_ZSt17__throw_bad_allocv;	.scl	2;	.type	32;	.endef
	.def	__cxa_begin_catch;	.scl	2;	.type	32;	.endef
	.def	__cxa_rethrow;	.scl	2;	.type	32;	.endef
	.def	__cxa_end_catch;	.scl	2;	.type	32;	.endef
	.def	_Unwind_Resume;	.scl	2;	.type	32;	.endef
	.def	memmove;	.scl	2;	.type	32;	.endef
	.def	_ZN4DeepeqERKNS_17ArchetypeBitFieldES2_;	.scl	2;	.type	32;	.endef
	.def	_ZN4Deep10ECRegistry17RegisterComponentEyyPKc;	.scl	2;	.type	32;	.endef
	.def	_ZN4Deep4ECDB12GetArchetypeEPiy;	.scl	2;	.type	32;	.endef
	.def	_ZN4Deep4ECDB9Archetype6EntityEv;	.scl	2;	.type	32;	.endef
	.def	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_x;	.scl	2;	.type	32;	.endef
	.def	_ZNSolsEi;	.scl	2;	.type	32;	.endef
	.def	_ZNSo3putEc;	.scl	2;	.type	32;	.endef
	.def	_ZN4Deep4ECDBD1Ev;	.scl	2;	.type	32;	.endef
	.def	_ZNKSt8__detail20_Prime_rehash_policy14_M_need_rehashEyyy;	.scl	2;	.type	32;	.endef
	.section	.rdata$.refptr._ZSt4cout, "dr"
	.globl	.refptr._ZSt4cout
	.linkonce	discard
.refptr._ZSt4cout:
	.quad	_ZSt4cout
