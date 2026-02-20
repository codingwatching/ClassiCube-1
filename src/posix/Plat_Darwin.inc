/* Field names differ depending on __DARWIN_UNIX03 */
#include <signal.h>
#include <sys/ucontext.h>

#if __DARWIN_UNIX03
/* See /usr/include/mach/i386/_structs.h (macOS 10.5+) */
void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	mcontext_t r = ((ucontext_t*)ctx)->uc_mcontext;
#if defined __i386__
	#define REG_GET(reg, ign) &r->__ss.__e##reg
	Dump_X86()
#elif defined __x86_64__
	#define REG_GET(reg, ign) &r->__ss.__r##reg
	Dump_X64()
#elif defined __aarch64__
	#define REG_GNUM(num)     &r->__ss.__x[num]
	#define REG_GET_FP()      &r->__ss.__fp
	#define REG_GET_LR()      &r->__ss.__lr
	#define REG_GET_SP()      &r->__ss.__sp
	#define REG_GET_PC()      &r->__ss.__pc
	Dump_ARM64()
#elif defined __arm__
	#define REG_GNUM(num)     &r->__ss.__r[num]
	#define REG_GET_FP()      &r->__ss.__r[11]
	#define REG_GET_IP()      &r->__ss.__r[12]
	#define REG_GET_SP()      &r->__ss.__sp
	#define REG_GET_LR()      &r->__ss.__lr
	#define REG_GET_PC()      &r->__ss.__pc
	Dump_ARM32()
#elif defined __ppc__
	#define REG_GNUM(num)     &r->__ss.__r##num
	#define REG_GET_PC()      &r->__ss.__srr0
	#define REG_GET_LR()      &r->__ss.__lr
	#define REG_GET_CTR()     &r->__ss.__ctr
	Dump_PPC()
#else
	#error "Unknown CPU architecture"
#endif
}
#else
/* See /usr/include/mach/i386/thread_status.h (macOS 10.4) */
void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	mcontext_t r = ((ucontext_t*)ctx)->uc_mcontext;
#if defined __i386__
	#define REG_GET(reg, ign) &r->ss.e##reg
	Dump_X86()
#elif defined __x86_64__
	#define REG_GET(reg, ign) &r->ss.r##reg
	Dump_X64()
#elif defined __ppc__
	#define REG_GNUM(num)     &r->ss.r##num
	#define REG_GET_PC()      &r->ss.srr0
	#define REG_GET_LR()      &r->ss.lr
	#define REG_GET_CTR()     &r->ss.ctr
	Dump_PPC()
#else
	#error "Unknown CPU architecture"
#endif
}
