/* See /usr/include/sys/ucontext.h */

/* Need to define this to get REG_ constants */
#define _GNU_SOURCE
#include <sys/ucontext.h>
#include <signal.h>

void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	mcontext_t* r = &((ucontext_t*)ctx)->uc_mcontext;

#if defined __i386__
	String_Format3(str, "eax=%x ebx=%x ecx=%x" _NL, &r->gregs[REG_EAX], &r->gregs[REG_EBX], &r->gregs[REG_ECX]);
	String_Format3(str, "edx=%x esi=%x edi=%x" _NL, &r->gregs[REG_EDX], &r->gregs[REG_ESI], &r->gregs[REG_EDI]);
	String_Format3(str, "eip=%x ebp=%x esp=%x" _NL, &r->gregs[REG_EIP], &r->gregs[REG_EBP], &r->gregs[REG_ESP]);
#elif defined __x86_64__
	String_Format3(str, "rax=%x rbx=%x rcx=%x" _NL, &r->gregs[REG_RAX], &r->gregs[REG_RBX], &r->gregs[REG_RCX]);
	String_Format3(str, "rdx=%x rsi=%x rdi=%x" _NL, &r->gregs[REG_RDX], &r->gregs[REG_RSI], &r->gregs[REG_RDI]);
	String_Format3(str, "rip=%x rbp=%x rsp=%x" _NL, &r->gregs[REG_RIP], &r->gregs[REG_RBP], &r->gregs[REG_RSP]);
	String_Format3(str, "r8 =%x r9 =%x r10=%x" _NL, &r->gregs[REG_R8],  &r->gregs[REG_R9],  &r->gregs[REG_R10]);
	String_Format3(str, "r11=%x r12=%x r13=%x" _NL, &r->gregs[REG_R11], &r->gregs[REG_R12], &r->gregs[REG_R13]);
	String_Format2(str, "r14=%x r15=%x" _NL,        &r->gregs[REG_R14], &r->gregs[REG_R15]);
#elif defined __aarch64__
	String_Format4(str, "r0 =%x r1 =%x r2 =%x r3 =%x" _NL, &r->regs[0],  &r->regs[1],  &r->regs[2],  &r->regs[3]);
	String_Format4(str, "r4 =%x r5 =%x r6 =%x r7 =%x" _NL, &r->regs[4],  &r->regs[5],  &r->regs[6],  &r->regs[7]);
	String_Format4(str, "r8 =%x r9 =%x r10=%x r11=%x" _NL, &r->regs[8],  &r->regs[9],  &r->regs[10], &r->regs[11]);
	String_Format4(str, "r12=%x r13=%x r14=%x r15=%x" _NL, &r->regs[12], &r->regs[13], &r->regs[14], &r->regs[15]);
	String_Format4(str, "r16=%x r17=%x r18=%x r19=%x" _NL, &r->regs[16], &r->regs[17], &r->regs[18], &r->regs[19]);
	String_Format4(str, "r20=%x r21=%x r22=%x r23=%x" _NL, &r->regs[20], &r->regs[21], &r->regs[22], &r->regs[23]);
	String_Format4(str, "r24=%x r25=%x r26=%x r27=%x" _NL, &r->regs[24], &r->regs[25], &r->regs[26], &r->regs[27];
	String_Format4(str, "r28=%x fp =%x lr =%x sp =%x" _NL, &r->regs[28], &r->regs[29], &r->regs[30], &r->sp);
	String_Format1(str, "pc =%x" _NL,                      &r->pc);
#elif defined __arm__
	String_Format3(str, "r0 =%x r1 =%x r2 =%x" _NL, &r->arm_r0, &r->arm_r1,  &r->arm_r2);
	String_Format3(str, "r3 =%x r4 =%x r5 =%x" _NL, &r->arm_r3, &r->arm_r4,  &r->arm_r5);
	String_Format3(str, "r6 =%x r7 =%x r8 =%x" _NL, &r->arm_r6, &r->arm_r7,  &r->arm_r8);
	String_Format3(str, "r9 =%x r10=%x fp =%x" _NL, &r->arm_r9, &r->arm_r10, &r->arm_fp);
	String_Format3(str, "ip =%x sp =%x lr =%x" _NL, &r->arm_ip, &r->arm_sp,  &r->arm_lr);
	String_Format1(str, "pc =%x"               _NL, &r->arm_pc);
#else
	#error "Unknown CPU architecture"
#endif
}
