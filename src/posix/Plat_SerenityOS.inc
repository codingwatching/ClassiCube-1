/* See Kernel/Arch/[ARCH]/mcontext.h */

/* Register constants can be found from includes in <signal.h> */
#include <signal.h>

void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	mcontext_t* r = &((ucontext_t*)ctx)->uc_mcontext;
#if defined __i386__
	#define REG_GET(reg, ign) &r->e##reg
	Dump_X86()
#elif defined __x86_64__
	#define REG_GET(reg, ign) &r->r##reg
	Dump_X64()
#else
	#error "Unknown CPU architecture"
#endif
}
