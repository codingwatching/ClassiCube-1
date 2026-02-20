/* See /usr/include/machine/signal.h */
/* -> src/sys/arch/[ARCH]/include/signal.h */

/* Register constants can be found from includes in <signal.h> */
#include <signal.h>

void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	ucontext_t* r = (ucontext_t*)ctx;
#if defined __i386__
	#define REG_GET(reg, ign) &r->sc_e##reg
	Dump_X86()
#elif defined __x86_64__
	#define REG_GET(reg, ign) &r->sc_r##reg
	Dump_X64()
#elif defined __aarch64__
	#define REG_GNUM(num)     &r->sc_x[num]
	#define REG_GET_FP()      &r->sc_x[29]
	#define REG_GET_LR()      &r->sc_lr
	#define REG_GET_SP()      &r->sc_sp
	#define REG_GET_PC()      &r->sc_elr
	Dump_ARM64()
#elif defined __arm__
	#define REG_GNUM(num)     &r->sc_r##num
	#define REG_GET_FP()      &r->sc_r11
	#define REG_GET_IP()      &r->sc_r12
	#define REG_GET_SP()      &r->sc_usr_sp
	#define REG_GET_LR()      &r->sc_usr_lr
	#define REG_GET_PC()      &r->sc_pc
	Dump_ARM32()
#elif defined __powerpc__
	#define REG_GNUM(num)     &r->sc_frame.fixreg[num]
	#define REG_GET_PC()      &r->sc_frame.srr0
	#define REG_GET_LR()      &r->sc_frame.lr
	#define REG_GET_CTR()     &r->sc_frame.ctr
	Dump_PPC()
#elif defined __mips__
	#define REG_GNUM(num)     &r->sc_regs[num]
	#define REG_GET_PC()      &r->sc_pc
	#define REG_GET_LO()      &r->mullo
	#define REG_GET_HI()      &r->mulhi
	Dump_MIPS()
#else
	#error "Unknown CPU architecture"
#endif
}
