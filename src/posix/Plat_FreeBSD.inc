/* See /usr/include/machine/ucontext.h */
/* -> src/sys/[ARCH]/include/ucontext.h */
#include <signal.h>
#include <sys/ucontext.h>

void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	mcontext_t* r = &((ucontext_t*)ctx)->uc_mcontext;
#if defined __i386__
	String_Format3(str, "eax=%x ebx=%x ecx=%x" _NL, &r->mc_eax, &r->mc_ebx, &r->mc_ecx);
	String_Format3(str, "edx=%x esi=%x edi=%x" _NL, &r->mc_edx, &r->mc_esi, &r->mc_edi);
	String_Format3(str, "eip=%x ebp=%x esp=%x" _NL, &r->mc_eip, &r->mc_ebp, &r->mc_esp);
#elif defined __x86_64__
	String_Format3(str, "rax=%x rbx=%x rcx=%x" _NL, &r->mc_rax, &r->mc_rbx, &r->mc_rcx);
	String_Format3(str, "rdx=%x rsi=%x rdi=%x" _NL, &r->mc_rdx, &r->mc_rsi, &r->mc_rdi);
	String_Format3(str, "rip=%x rbp=%x rsp=%x" _NL, &r->mc_rip, &r->mc_rbp, &r->mc_rsp);
	String_Format3(str, "r8 =%x r9 =%x r10=%x" _NL, &r->mc_r8,  &r->mc_r9,  &r->mc_r10);
	String_Format3(str, "r11=%x r12=%x r13=%x" _NL, &r->mc_r11, &r->mc_r12, &r->mc_r13);
	String_Format2(str, "r14=%x r15=%x" _NL,        &r->mc_r14, &r->mc_r15);
#elif defined __aarch64__
	String_Format4(str, "r0 =%x r1 =%x r2 =%x r3 =%x" _NL, &r->mc_gpregs.gp_x[0],  &r->mc_gpregs.gp_x[1],  &r->mc_gpregs.gp_x[2],  &r->mc_gpregs.gp_x[3]);
	String_Format4(str, "r4 =%x r5 =%x r6 =%x r7 =%x" _NL, &r->mc_gpregs.gp_x[4],  &r->mc_gpregs.gp_x[5],  &r->mc_gpregs.gp_x[6],  &r->mc_gpregs.gp_x[7]);
	String_Format4(str, "r8 =%x r9 =%x r10=%x r11=%x" _NL, &r->mc_gpregs.gp_x[8],  &r->mc_gpregs.gp_x[9],  &r->mc_gpregs.gp_x[10], &r->mc_gpregs.gp_x[11]);
	String_Format4(str, "r12=%x r13=%x r14=%x r15=%x" _NL, &r->mc_gpregs.gp_x[12], &r->mc_gpregs.gp_x[13], &r->mc_gpregs.gp_x[14], &r->mc_gpregs.gp_x[15]);
	String_Format4(str, "r16=%x r17=%x r18=%x r19=%x" _NL, &r->mc_gpregs.gp_x[16], &r->mc_gpregs.gp_x[17], &r->mc_gpregs.gp_x[18], &r->mc_gpregs.gp_x[19]);
	String_Format4(str, "r20=%x r21=%x r22=%x r23=%x" _NL, &r->mc_gpregs.gp_x[20], &r->mc_gpregs.gp_x[21], &r->mc_gpregs.gp_x[22], &r->mc_gpregs.gp_x[23]);
	String_Format4(str, "r24=%x r25=%x r26=%x r27=%x" _NL, &r->mc_gpregs.gp_x[24], &r->mc_gpregs.gp_x[25], &r->mc_gpregs.gp_x[26], &r->mc_gpregs.gp_x[27]);
	String_Format4(str, "r28=%x r29=%x lr =%x sp =%x" _NL, &r->mc_gpregs.gp_x[28], &r->mc_gpregs.gp_x[29], &r->mc_gpregs.gp_lr,    &r->mc_gpregs.gp_sp);
	String_Format1(str, "pc =%x" _NL,                      &r->mc_gpregs.gp_elr);
#elif defined __arm__
	String_Format3(str, "r0 =%x r1 =%x r2 =%x" _NL, &r->__gregs[0],      &r->__gregs[1],       &r->__gregs[2]);
	String_Format3(str, "r3 =%x r4 =%x r5 =%x" _NL, &r->__gregs[3],      &r->__gregs[4],       &r->__gregs[5]);
	String_Format3(str, "r6 =%x r7 =%x r8 =%x" _NL, &r->__gregs[6],      &r->__gregs[7],       &r->__gregs[8]);
	String_Format3(str, "r9 =%x r10=%x fp =%x" _NL, &r->__gregs[9],      &r->__gregs[10],      &r->__gregs[_REG_FP]);
	String_Format3(str, "r12=%x sp =%x lr =%x" _NL, &r->__gregs[12],     &r->__gregs[_REG_SP], &r->__gregs[_REG_LR]);
	String_Format1(str, "pc =%x"               _NL, &r->__gregs[_REG_PC]);
#elif defined __powerpc__
	String_Format4(str, "r0 =%x r1 =%x r2 =%x r3 =%x" _NL, &r->mc_frame[0],  &r->mc_frame[1],  &r->mc_frame[2],  &r->mc_frame[3]);
	String_Format4(str, "r4 =%x r5 =%x r6 =%x r7 =%x" _NL, &r->mc_frame[4],  &r->mc_frame[5],  &r->mc_frame[6],  &r->mc_frame[7]);
	String_Format4(str, "r8 =%x r9 =%x r10=%x r11=%x" _NL, &r->mc_frame[8],  &r->mc_frame[9],  &r->mc_frame[10], &r->mc_frame[11]);
	String_Format4(str, "r12=%x r13=%x r14=%x r15=%x" _NL, &r->mc_frame[12], &r->mc_frame[13], &r->mc_frame[14], &r->mc_frame[15]);
	String_Format4(str, "r16=%x r17=%x r18=%x r19=%x" _NL, &r->mc_frame[16], &r->mc_frame[17], &r->mc_frame[18], &r->mc_frame[19]);
	String_Format4(str, "r20=%x r21=%x r22=%x r23=%x" _NL, &r->mc_frame[20], &r->mc_frame[21], &r->mc_frame[22], &r->mc_frame[23]);
	String_Format4(str, "r24=%x r25=%x r26=%x r27=%x" _NL, &r->mc_frame[24], &r->mc_frame[25], &r->mc_frame[26], &r->mc_frame[27]);
	String_Format4(str, "r28=%x r29=%x r30=%x r31=%x" _NL, &r->mc_frame[28], &r->mc_frame[29], &r->mc_frame[30], &r->mc_frame[31]);
	String_Format3(str, "pc =%x lr =%x ctr=%x" _NL,        &r->mc_srr0,      &r->mc_lr,        &r->mc_ctr);
#elif defined __mips__
	String_Format4(str, "r1 =%x r2 =%x r3 =%x r4 =%x" _NL, &r->mc_regs[1],  &r->mc_regs[2],  &r->mc_regs[3],  &r->mc_regs[4]);
	String_Format4(str, "r5 =%x r6 =%x r7 =%x r8 =%x" _NL, &r->mc_regs[5],  &r->mc_regs[6],  &r->mc_regs[7],  &r->mc_regs[8]);
	String_Format4(str, "r9 =%x r10=%x r11=%x r12=%x" _NL, &r->mc_regs[9],  &r->mc_regs[10], &r->mc_regs[11], &r->mc_regs[12]);
	String_Format4(str, "r13=%x r14=%x r15=%x r16=%x" _NL, &r->mc_regs[13], &r->mc_regs[14], &r->mc_regs[15], &r->mc_regs[16]);
	String_Format4(str, "r17=%x r18=%x r19=%x r20=%x" _NL, &r->mc_regs[17], &r->mc_regs[18], &r->mc_regs[19], &r->mc_regs[20]);
	String_Format4(str, "r21=%x r22=%x r23=%x r24=%x" _NL, &r->mc_regs[21], &r->mc_regs[22], &r->mc_regs[23], &r->mc_regs[24]);
	String_Format4(str, "r25=%x r26=%x r27=%x r28=%x" _NL, &r->mc_regs[25], &r->mc_regs[26], &r->mc_regs[27], &r->mc_regs[28]);
	String_Format4(str, "sp =%x fp =%x ra =%x pc =%x" _NL, &r->mc_regs[29], &r->mc_regs[30], &r->mc_regs[31], &r->mc_pc);
	String_Format3(str, "lo =%x hi =%x" _NL,               &r->mullo,       &r->mullhi);
#else
	#error "Unknown CPU architecture"
#endif
}
