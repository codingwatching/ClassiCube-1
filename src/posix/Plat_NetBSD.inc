/* See /usr/include/[ARCH]/mcontext.h */
/* -> src/sys/arch/[ARCH]/include/mcontext.h */
#include <signal.h>
#include <sys/ucontext.h>

void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	mcontext_t* r = &((ucontext_t*)ctx)->uc_mcontext;
#if defined __i386__
	String_Format3(str, "eax=%x ebx=%x ecx=%x" _NL, &r->__gregs[_REG_EAX], &r->__gregs[_REG_EBX], &r->__gregs[_REG_ECX]);
	String_Format3(str, "edx=%x esi=%x edi=%x" _NL, &r->__gregs[_REG_EDX], &r->__gregs[_REG_ESI], &r->__gregs[_REG_EDI]);
	String_Format3(str, "eip=%x ebp=%x esp=%x" _NL, &r->__gregs[_REG_EIP], &r->__gregs[_REG_EBP], &r->__gregs[_REG_ESP]);
#elif defined __x86_64__
	String_Format3(str, "rax=%x rbx=%x rcx=%x" _NL, &r->__gregs[_REG_RAX], &r->__gregs[_REG_RBX], &r->__gregs[_REG_RCX]);
	String_Format3(str, "rdx=%x rsi=%x rdi=%x" _NL, &r->__gregs[_REG_RDX], &r->__gregs[_REG_RSI], &r->__gregs[_REG_RDI]);
	String_Format3(str, "rip=%x rbp=%x rsp=%x" _NL, &r->__gregs[_REG_RIP], &r->__gregs[_REG_RBP], &r->__gregs[_REG_RSP]);
	String_Format3(str, "r8 =%x r9 =%x r10=%x" _NL, &r->__gregs[_REG_R8],  &r->__gregs[_REG_R9],  &r->__gregs[_REG_R10]);
	String_Format3(str, "r11=%x r12=%x r13=%x" _NL, &r->__gregs[_REG_R11], &r->__gregs[_REG_R12], &r->__gregs[_REG_R13]);
	String_Format2(str, "r14=%x r15=%x" _NL,        &r->__gregs[_REG_R14], &r->__gregs[_REG_R15]);
#elif defined __aarch64__
	String_Format4(str, "r0 =%x r1 =%x r2 =%x r3 =%x" _NL, &r->__gregs[0],      &r->__gregs[1],       &r->__gregs[2],       &r->__gregs[3]);
	String_Format4(str, "r4 =%x r5 =%x r6 =%x r7 =%x" _NL, &r->__gregs[4],      &r->__gregs[5],       &r->__gregs[6],       &r->__gregs[7]);
	String_Format4(str, "r8 =%x r9 =%x r10=%x r11=%x" _NL, &r->__gregs[8],      &r->__gregs[9],       &r->__gregs[10],      &r->__gregs[11]);
	String_Format4(str, "r12=%x r13=%x r14=%x r15=%x" _NL, &r->__gregs[12],     &r->__gregs[13],      &r->__gregs[14],      &r->__gregs[15]);
	String_Format4(str, "r16=%x r17=%x r18=%x r19=%x" _NL, &r->__gregs[16],     &r->__gregs[17],      &r->__gregs[18],      &r->__gregs[19]);
	String_Format4(str, "r20=%x r21=%x r22=%x r23=%x" _NL, &r->__gregs[20],     &r->__gregs[21],      &r->__gregs[22],      &r->__gregs[23]);
	String_Format4(str, "r24=%x r25=%x r26=%x r27=%x" _NL, &r->__gregs[24],     &r->__gregs[25],      &r->__gregs[26],      &r->__gregs[27]);
	String_Format4(str, "r28=%x fp =%x lr =%x sp =%x" _NL, &r->__gregs[28],     &r->__gregs[_REG_FP], &r->__gregs[_REG_LR], &r->__gregs[_REG_SP]);
	String_Format1(str, "pc =%x" _NL,                      &r->__gregs[_REG_PC]);
#elif defined __arm__
	String_Format3(str, "r0 =%x r1 =%x r2 =%x" _NL, &r->__gregs[0],      &r->__gregs[1],       &r->__gregs[2]);
	String_Format3(str, "r3 =%x r4 =%x r5 =%x" _NL, &r->__gregs[3],      &r->__gregs[4],       &r->__gregs[5]);
	String_Format3(str, "r6 =%x r7 =%x r8 =%x" _NL, &r->__gregs[6],      &r->__gregs[7],       &r->__gregs[8]);
	String_Format3(str, "r9 =%x r10=%x fp =%x" _NL, &r->__gregs[9],      &r->__gregs[10],      &r->__gregs[_REG_FP]);
	String_Format3(str, "r12=%x sp =%x lr =%x" _NL, &r->__gregs[12],     &r->__gregs[_REG_SP], &r->__gregs[_REG_LR]);
	String_Format1(str, "pc =%x"               _NL, &r->__gregs[_REG_PC]);
#elif defined __powerpc__
	String_Format4(str, "r0 =%x r1 =%x r2 =%x r3 =%x" _NL, &r->__gregs[0],       &r->__gregs[1],       &r->__gregs[2],       &r->__gregs[3]);
	String_Format4(str, "r4 =%x r5 =%x r6 =%x r7 =%x" _NL, &r->__gregs[4],       &r->__gregs[5],       &r->__gregs[6],       &r->__gregs[7]);
	String_Format4(str, "r8 =%x r9 =%x r10=%x r11=%x" _NL, &r->__gregs[8],       &r->__gregs[9],       &r->__gregs[10],      &r->__gregs[11]);
	String_Format4(str, "r12=%x r13=%x r14=%x r15=%x" _NL, &r->__gregs[12],      &r->__gregs[13],      &r->__gregs[14],      &r->__gregs[15]);
	String_Format4(str, "r16=%x r17=%x r18=%x r19=%x" _NL, &r->__gregs[16],      &r->__gregs[17],      &r->__gregs[18],      &r->__gregs[19]);
	String_Format4(str, "r20=%x r21=%x r22=%x r23=%x" _NL, &r->__gregs[20],      &r->__gregs[21],      &r->__gregs[22],      &r->__gregs[23]);
	String_Format4(str, "r24=%x r25=%x r26=%x r27=%x" _NL, &r->__gregs[24],      &r->__gregs[25],      &r->__gregs[26],      &r->__gregs[27]);
	String_Format4(str, "r28=%x r29=%x r30=%x r31=%x" _NL, &r->__gregs[28],      &r->__gregs[29],      &r->__gregs[30],      &r->__gregs[31]);
	String_Format3(str, "pc =%x lr =%x ctr=%x" _NL,        &r->__gregs[_REG_PC], &r->__gregs[_REG_LR], &r->__gregs[_REG_CTR]);
#elif defined __sparc__
	String_Format4(str, "o0=%x o1=%x o2=%x o3=%x" _NL, &r->__gregs[_REG_O0], &r->__gregs[_REG_O1], &r->__gregs[_REG_O2], &r->__gregs[_REG_O3]);
	String_Format4(str, "o4=%x o5=%x o6=%x o7=%x" _NL, &r->__gregs[_REG_O4], &r->__gregs[_REG_O5], &r->__gregs[_REG_O6], &r->__gregs[_REG_O7]);
	String_Format4(str, "g1=%x g2=%x g3=%x g4=%x" _NL, &r->__gregs[_REG_G1], &r->__gregs[_REG_G2], &r->__gregs[_REG_G3], &r->__gregs[_REG_G4]);
	String_Format4(str, "g5=%x g6=%x g7=%x y =%x" _NL, &r->__gregs[_REG_G5], &r->__gregs[_REG_G6], &r->__gregs[_REG_G7], &r->__gregs[_REG_Y]);
	String_Format2(str, "pc=%x nc=%x" _NL,             &r->__gregs[_REG_PC], &r->__gregs[_REG_nPC]);
#elif defined __mips__
	String_Format4(str, "r1 =%x r2 =%x r3 =%x r4 =%x" _NL, &r->__gregs[1],         &r->__gregs[2],         &r->__gregs[3],  &r->__gregs[4]);
	String_Format4(str, "r5 =%x r6 =%x r7 =%x r8 =%x" _NL, &r->__gregs[5],         &r->__gregs[6],         &r->__gregs[7],  &r->__gregs[8]);
	String_Format4(str, "r9 =%x r10=%x r11=%x r12=%x" _NL, &r->__gregs[9],         &r->__gregs[10],        &r->__gregs[11], &r->__gregs[12]);
	String_Format4(str, "r13=%x r14=%x r15=%x r16=%x" _NL, &r->__gregs[13],        &r->__gregs[14],        &r->__gregs[15], &r->__gregs[16]);
	String_Format4(str, "r17=%x r18=%x r19=%x r20=%x" _NL, &r->__gregs[17],        &r->__gregs[18],        &r->__gregs[19], &r->__gregs[20]);
	String_Format4(str, "r21=%x r22=%x r23=%x r24=%x" _NL, &r->__gregs[21],        &r->__gregs[22],        &r->__gregs[23], &r->__gregs[24]);
	String_Format4(str, "r25=%x r26=%x r27=%x r28=%x" _NL, &r->__gregs[25],        &r->__gregs[26],        &r->__gregs[27], &r->__gregs[28]);
	String_Format4(str, "sp =%x fp =%x ra =%x pc =%x" _NL, &r->__gregs[29],        &r->__gregs[30],        &r->__gregs[31], &r->__gregs[_REG_EPC]);
	String_Format3(str, "lo =%x hi =%x" _NL,               &r->__gregs[_REG_MDLO], &r->__gregs[_REG_MDHI]);
#elif defined __riscv
	String_Format4(str, "pc =%x r1 =%x r2 =%x r3 =%x" _NL, &r->__gregs[_REG_PC], &r->__gregs[1],  &r->__gregs[2],  &r->__gregs[3]);
	String_Format4(str, "r4 =%x r5 =%x r6 =%x r7 =%x" _NL, &r->__gregs[4],       &r->__gregs[5],  &r->__gregs[6],  &r->__gregs[7]);
	String_Format4(str, "r8 =%x r9 =%x r10=%x r11=%x" _NL, &r->__gregs[8],       &r->__gregs[9],  &r->__gregs[10], &r->__gregs[11]);
	String_Format4(str, "r12=%x r13=%x r14=%x r15=%x" _NL, &r->__gregs[12],      &r->__gregs[13], &r->__gregs[14], &r->__gregs[15]);
	String_Format4(str, "r16=%x r17=%x r18=%x r19=%x" _NL, &r->__gregs[16],      &r->__gregs[17], &r->__gregs[18], &r->__gregs[19]);
	String_Format4(str, "r20=%x r21=%x r22=%x r23=%x" _NL, &r->__gregs[20],      &r->__gregs[21], &r->__gregs[22], &r->__gregs[23]);
	String_Format4(str, "r24=%x r25=%x r26=%x r27=%x" _NL, &r->__gregs[24],      &r->__gregs[25], &r->__gregs[26], &r->__gregs[27]);
	String_Format4(str, "r28=%x r29=%x r30=%x r31=%x" _NL, &r->__gregs[28],      &r->__gregs[29], &r->__gregs[30], &r->__gregs[31]);
#else
	#error "Unknown CPU architecture"
#endif
}

