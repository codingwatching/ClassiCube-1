/* See /usr/include/machine/signal.h */
/* -> src/sys/arch/[ARCH]/include/signal.h */

/* Register constants can be found from includes in <signal.h> */
#include <signal.h>

void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	ucontext_t* r = (ucontext_t*)ctx;
#if defined __i386__
	String_Format3(str, "eax=%x ebx=%x ecx=%x" _NL, &r->sc_eax, &r->sc_ebx, &r->sc_ecx);
	String_Format3(str, "edx=%x esi=%x edi=%x" _NL, &r->sc_edx, &r->sc_esi, &r->sc_edi);
	String_Format3(str, "eip=%x ebp=%x esp=%x" _NL, &r->sc_eip, &r->sc_ebp, &r->sc_esp);
#elif defined __x86_64__
	String_Format3(str, "rax=%x rbx=%x rcx=%x" _NL, &r->sc_rax, &r->sc_rbx, &r->sc_rcx);
	String_Format3(str, "rdx=%x rsi=%x rdi=%x" _NL, &r->sc_rdx, &r->sc_rsi, &r->sc_rdi);
	String_Format3(str, "rip=%x rbp=%x rsp=%x" _NL, &r->sc_rip, &r->sc_rbp, &r->sc_rsp);
	String_Format3(str, "r8 =%x r9 =%x r10=%x" _NL, &r->sc_r8,  &r->sc_r9,  &r->sc_r10);
	String_Format3(str, "r11=%x r12=%x r13=%x" _NL, &r->sc_r11, &r->sc_r12, &r->sc_r13);
	String_Format2(str, "r14=%x r15=%x" _NL,        &r->sc_r14, &r->sc_r15);
#elif defined __aarch64__
	String_Format4(str, "r0 =%x r1 =%x r2 =%x r3 =%x" _NL, &r->sc_x[0],  &r->sc_x[1],  &r->sc_x[2],  &r->sc_x[3]);
	String_Format4(str, "r4 =%x r5 =%x r6 =%x r7 =%x" _NL, &r->sc_x[4],  &r->sc_x[5],  &r->sc_x[6],  &r->sc_x[7]);
	String_Format4(str, "r8 =%x r9 =%x r10=%x r11=%x" _NL, &r->sc_x[8],  &r->sc_x[9],  &r->sc_x[10], &r->sc_x[11]);
	String_Format4(str, "r12=%x r13=%x r14=%x r15=%x" _NL, &r->sc_x[12], &r->sc_x[13], &r->sc_x[14], &r->sc_x[15]);
	String_Format4(str, "r16=%x r17=%x r18=%x r19=%x" _NL, &r->sc_x[16], &r->sc_x[17], &r->sc_x[18], &r->sc_x[19]);
	String_Format4(str, "r20=%x r21=%x r22=%x r23=%x" _NL, &r->sc_x[20], &r->sc_x[21], &r->sc_x[22], &r->sc_x[23]);
	String_Format4(str, "r24=%x r25=%x r26=%x r27=%x" _NL, &r->sc_x[24], &r->sc_x[25], &r->sc_x[26], &r->sc_x[27]);
	String_Format4(str, "r28=%x r29=%x lr =%x sp =%x" _NL, &r->sc_x[28], &r->sc_x[29], &r->sc_lr,    &r->sc_sp);
	String_Format1(str, "pc =%x" _NL,                      &r->sc_elr);
#elif defined __arm__
	String_Format3(str, "r0 =%x r1 =%x r2 =%x" _NL, &r->sc_r0,  &r->sc_r1,     &r->sc_r2);
	String_Format3(str, "r3 =%x r4 =%x r5 =%x" _NL, &r->sc_r3,  &r->sc_r4,     &r->sc_r5);
	String_Format3(str, "r6 =%x r7 =%x r8 =%x" _NL, &r->sc_r6,  &r->sc_r7,     &r->sc_r8);
	String_Format3(str, "r9 =%x r10=%x r11=%x" _NL, &r->sc_r9,  &r->sc_r10,    &r->sc_r11);
	String_Format3(str, "r12=%x sp =%x lr =%x" _NL, &r->sc_r12, &r->sc_usr_sp, &r->sc_usr_lr)
	String_Format1(str, "pc =%x"               _NL, &r->sc_pc);
#elif defined __powerpc__
	String_Format4(str, "r0 =%x r1 =%x r2 =%x r3 =%x" _NL, &r->sc_frame.fixreg[0],  &r->sc_frame.fixreg[1],  &r->sc_frame.fixreg[2],  &r->sc_frame.fixreg[3]);
	String_Format4(str, "r4 =%x r5 =%x r6 =%x r7 =%x" _NL, &r->sc_frame.fixreg[4],  &r->sc_frame.fixreg[5],  &r->sc_frame.fixreg[6],  &r->sc_frame.fixreg[7]);
	String_Format4(str, "r8 =%x r9 =%x r10=%x r11=%x" _NL, &r->sc_frame.fixreg[8],  &r->sc_frame.fixreg[9],  &r->sc_frame.fixreg[10], &r->sc_frame.fixreg[11]);
	String_Format4(str, "r12=%x r13=%x r14=%x r15=%x" _NL, &r->sc_frame.fixreg[12], &r->sc_frame.fixreg[13], &r->sc_frame.fixreg[14], &r->sc_frame.fixreg[15]);
	String_Format4(str, "r16=%x r17=%x r18=%x r19=%x" _NL, &r->sc_frame.fixreg[16], &r->sc_frame.fixreg[17], &r->sc_frame.fixreg[18], &r->sc_frame.fixreg[19]);
	String_Format4(str, "r20=%x r21=%x r22=%x r23=%x" _NL, &r->sc_frame.fixreg[20], &r->sc_frame.fixreg[21], &r->sc_frame.fixreg[22], &r->sc_frame.fixreg[23]);
	String_Format4(str, "r24=%x r25=%x r26=%x r27=%x" _NL, &r->sc_frame.fixreg[24], &r->sc_frame.fixreg[25], &r->sc_frame.fixreg[26], &r->sc_frame.fixreg[27]);
	String_Format4(str, "r28=%x r29=%x r30=%x r31=%x" _NL, &r->sc_frame.fixreg[28], &r->sc_frame.fixreg[29], &r->sc_frame.fixreg[30], &r->sc_frame.fixreg[31]);
	String_Format3(str, "pc =%x lr =%x ctr=%x" _NL,        &r->sc_frame.srr0,       &r->sc_frame.lr,         &r->sc_frame.ctr);
#elif defined __mips__
	String_Format4(str, "r1 =%x r2 =%x r3 =%x r4 =%x" _NL, &r->sc_regs[1],  &r->sc_regs[2],  &r->sc_regs[3],  &r->sc_regs[4]);
	String_Format4(str, "r5 =%x r6 =%x r7 =%x r8 =%x" _NL, &r->sc_regs[5],  &r->sc_regs[6],  &r->sc_regs[7],  &r->sc_regs[8]);
	String_Format4(str, "r9 =%x r10=%x r11=%x r12=%x" _NL, &r->sc_regs[9],  &r->sc_regs[10], &r->sc_regs[11], &r->sc_regs[12]);
	String_Format4(str, "r13=%x r14=%x r15=%x r16=%x" _NL, &r->sc_regs[13], &r->sc_regs[14], &r->sc_regs[15], &r->sc_regs[16]);
	String_Format4(str, "r17=%x r18=%x r19=%x r20=%x" _NL, &r->sc_regs[17], &r->sc_regs[18], &r->sc_regs[19], &r->sc_regs[20]);
	String_Format4(str, "r21=%x r22=%x r23=%x r24=%x" _NL, &r->sc_regs[21], &r->sc_regs[22], &r->sc_regs[23], &r->sc_regs[24]);
	String_Format4(str, "r25=%x r26=%x r27=%x r28=%x" _NL, &r->sc_regs[25], &r->sc_regs[26], &r->sc_regs[27], &r->sc_regs[28]);
	String_Format4(str, "sp =%x fp =%x ra =%x pc =%x" _NL, &r->sc_regs[29], &r->sc_regs[30], &r->sc_regs[31], &r->sc_pc);
	String_Format3(str, "lo =%x hi =%x" _NL,               &r->mullo,       &r->mullhi);
#else
	#error "Unknown CPU architecture"
#endif
}
