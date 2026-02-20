/* See /headers/posix/arch/[ARCH]/signal.h */

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
#elif defined __aarch64__
	#define REG_GNUM(num)     &r->x[num]
	#define REG_GET_FP()      &r->x[29]
	#define REG_GET_LR()      &r->lr
	#define REG_GET_SP()      &r->sp
	#define REG_GET_PC()      &r->elr
	Dump_ARM64()
#elif defined __arm__
	#define REG_GNUM(num)     &r->r##num
	#define REG_GET_FP()      &r->r11
	#define REG_GET_IP()      &r->r12
	#define REG_GET_SP()      &r->r13
	#define REG_GET_LR()      &r->r14
	#define REG_GET_PC()      &r->r15
	Dump_ARM32()
#elif defined __riscv
	#define REG_GNUM(num)     &r->x[num - 1]
	#define REG_GET_PC()      &r->pc
	Dump_RISCV()
#else
	#error "Unknown CPU architecture"
#endif
}
