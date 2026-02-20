/* See /usr/include/sys/ucontext.h */

/* Need to define this to get REG_ constants */
#define _GNU_SOURCE
#include <sys/ucontext.h>
#include <signal.h>

void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	mcontext_t* r = &((ucontext_t*)ctx)->uc_mcontext;

#if defined __i386__
	#define REG_GET(ign, reg) &r->gregs[REG_E##reg]
	Dump_X86()
#elif defined __x86_64__
	#define REG_GET(ign, reg) &r->gregs[REG_R##reg]
	Dump_X64()
#elif defined __aarch64__
	#define REG_GNUM(num)     &r->regs[num]
	#define REG_GET_FP()      &r->regs[29]
	#define REG_GET_LR()      &r->regs[30]
	#define REG_GET_SP()      &r->sp
	#define REG_GET_PC()      &r->pc
	Dump_ARM64()
#elif defined __arm__
	#define REG_GNUM(num)     &r->arm_r##num
	#define REG_GET_FP()      &r->arm_fp
	#define REG_GET_IP()      &r->arm_ip
	#define REG_GET_SP()      &r->arm_sp
	#define REG_GET_LR()      &r->arm_lr
	#define REG_GET_PC()      &r->arm_pc
	Dump_ARM32()
#else
	#error "Unknown CPU architecture"
#endif
}
