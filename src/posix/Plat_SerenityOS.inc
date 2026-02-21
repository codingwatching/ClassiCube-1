/* See Kernel/Arch/[ARCH]/mcontext.h */

/* Register constants can be found from includes in <signal.h> */
#include <signal.h>

#define RREG(reg) &r->reg

void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	mcontext_t* r = &((ucontext_t*)ctx)->uc_mcontext;
#if defined __i386__
	String_Format3(str, "eax=%x ebx=%x ecx=%x" _NL, RREG(eax), RREG(ebx), RREG(ecx));
	String_Format3(str, "edx=%x esi=%x edi=%x" _NL, RREG(edx), RREG(esi), RREG(edi));
	String_Format3(str, "eip=%x ebp=%x esp=%x" _NL, RREG(eip), RREG(ebp), RREG(esp));
#elif defined __x86_64__
	String_Format3(str, "rax=%x rbx=%x rcx=%x" _NL, RREG(rax), RREG(rbx), RREG(rcx));
	String_Format3(str, "rdx=%x rsi=%x rdi=%x" _NL, RREG(rdx), RREG(rsi), RREG(rdi));
	String_Format3(str, "rip=%x rbp=%x rsp=%x" _NL, RREG(rip), RREG(rbp), RREG(rsp));
	String_Format3(str, "r8 =%x r9 =%x r10=%x" _NL, RREG(r8),  RREG(r9),  RREG(r10));
	String_Format3(str, "r11=%x r12=%x r13=%x" _NL, RREG(r11), RREG(r12), RREG(r13));
	String_Format2(str, "r14=%x r15=%x" _NL,        RREG(r14), RREG(r15));
#else
	#error "Unknown CPU architecture"
#endif
}
