/* See Kernel/Arch/[ARCH]/mcontext.h */

/* Register constants can be found from includes in <signal.h> */
#include <signal.h>

void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	mcontext_t* r = &((ucontext_t*)ctx)->uc_mcontext;
#if defined __i386__
	String_Format3(str, "eax=%x ebx=%x ecx=%x" _NL, &r->eax, &r->ebx, &r->ecx);
	String_Format3(str, "edx=%x esi=%x edi=%x" _NL, &r->edx, &r->esi, &r->edi);
	String_Format3(str, "eip=%x ebp=%x esp=%x" _NL, &r->eip, &r->ebp, &r->esp);
#elif defined __x86_64__
	String_Format3(str, "rax=%x rbx=%x rcx=%x" _NL, &r->rax, &r->rbx, &r->rcx);
	String_Format3(str, "rdx=%x rsi=%x rdi=%x" _NL, &r->rdx, &r->rsi, &r->rdi);
	String_Format3(str, "rip=%x rbp=%x rsp=%x" _NL, &r->rip, &r->rbp, &r->rsp);
	String_Format3(str, "r8 =%x r9 =%x r10=%x" _NL, &r->r8,  &r->r9,  &r->r10);
	String_Format3(str, "r11=%x r12=%x r13=%x" _NL, &r->r11, &r->r12, &r->r13);
	String_Format2(str, "r14=%x r15=%x" _NL,        &r->r14, &r->r15);
#else
	#error "Unknown CPU architecture"
#endif
}
