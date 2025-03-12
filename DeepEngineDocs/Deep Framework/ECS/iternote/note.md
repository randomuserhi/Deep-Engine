```cpp
void Loop1(std::vector<Component>& comps) {
    for (size_t i = 0; i < comps.size(); ++i) {
        Component& t = comps[i];
        t.a = i;
    }
}

void Loop2(Deep::ECDB::Archetype& arch, Deep::ComponentId comp) {
    Deep::ECDB::Archetype::ComponentOffset offset = arch.GetComponentOffset(comp);
    for (Deep::ECDB::Archetype::Chunk* c = arch.chunks(); c != nullptr; c = c->next) {
        Component* comps = arch.GetCompList<Component>(c, offset);
        for (size_t i = 0; i < arch.GetChunkSize(c); ++i) {
            Component& t = comps[i];
            t.a = i;
        }
    }
}
```

Loop1 main loop compiles to:
```asm
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
```

Loop2 loop compiles to:
```asm
// Iterating through chunks
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

// MAIN LOOP
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

// Iterating through chunks
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
```

The main loop compiles into nice vectorized code! (Tested `g++ -std=c++17`)