/* See /usr/include/machine/ucontext.h */
/* -> src/sys/[ARCH]/include/ucontext.h */
#include <signal.h>
#include <sys/ucontext.h>

void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	mcontext_t* r = &((ucontext_t*)ctx)->uc_mcontext;
#if defined __i386__
	#define REG_GET(reg, ign) &r->mc_e##reg
	Dump_X86()
#elif defined __x86_64__
	#define REG_GET(reg, ign) &r->mc_r##reg
	Dump_X64()
#elif defined __aarch64__
	#define REG_GNUM(num)     &r->mc_gpregs.gp_x[num]
	#define REG_GET_FP()      &r->mc_gpregs.gp_x[29]
	#define REG_GET_LR()      &r->mc_gpregs.gp_lr
	#define REG_GET_SP()      &r->mc_gpregs.gp_sp
	#define REG_GET_PC()      &r->mc_gpregs.gp_elr
	Dump_ARM64()
#elif defined __arm__
	#define REG_GNUM(num)     &r->__gregs[num]
	#define REG_GET_FP()      &r->__gregs[_REG_FP]
	#define REG_GET_IP()      &r->__gregs[12]
	#define REG_GET_SP()      &r->__gregs[_REG_SP]
	#define REG_GET_LR()      &r->__gregs[_REG_LR]
	#define REG_GET_PC()      &r->__gregs[_REG_PC]
	Dump_ARM32()
#elif defined __powerpc__
	#define REG_GNUM(num)     &r->mc_frame[##num]
	#define REG_GET_PC()      &r->mc_srr0
	#define REG_GET_LR()      &r->mc_lr
	#define REG_GET_CTR()     &r->mc_ctr
	Dump_PPC()
#elif defined __mips__
	#define REG_GNUM(num)     &r->mc_regs[num]
	#define REG_GET_PC()      &r->mc_pc
	#define REG_GET_LO()      &r->mullo
	#define REG_GET_HI()      &r->mulhi
	Dump_MIPS()
#else
	#error "Unknown CPU architecture"
#endif
}
