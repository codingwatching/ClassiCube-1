/* See /usr/include/sys/regset.h */
/* -> usr/src/uts/[ARCH]/sys/mcontext.h */
/* -> usr/src/uts/[ARCH]/sys/regset.h */
#include <signal.h>
#include <sys/ucontext.h>
#include <sys/regset.h>

void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	mcontext_t* r = &((ucontext_t*)ctx)->uc_mcontext;

#if defined __i386__
	#define REG_GET(ign, reg) &r->gregs[E##reg]
	Dump_X86()
#elif defined __x86_64__
	#define REG_GET(ign, reg) &r->gregs[REG_R##reg]
	Dump_X64()
#elif defined __sparc__
	#define REG_GET(ign, reg) &r->gregs[REG_##reg]
	Dump_SPARC()
#else
	#error "Unknown CPU architecture"
#endif
}
