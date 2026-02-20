/* See /usr/include/sys/ucontext.h */

/* Need to define this to get REG_ constants */
#define _GNU_SOURCE
#include <sys/ucontext.h>
#include <signal.h>

void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
#if __PPC__ && __WORDSIZE == 32
	/* See sysdeps/unix/sysv/linux/powerpc/sys/ucontext.h in glibc */
	mcontext_t* r = ((ucontext_t*)ctx)->uc_mcontext.uc_regs;
#elif __e2k__
	struct sigcontext* r = &((ucontext_t*)ctx)->uc_mcontext;
#else
	mcontext_t* r = &((ucontext_t*)ctx)->uc_mcontext;
#endif

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
#elif defined __alpha__
	#define REG_GNUM(num)     &r->sc_regs[num]
	#define REG_GET_FP()      &r->sc_regs[15]
	#define REG_GET_PC()      &r->sc_pc
	#define REG_GET_SP()      &r->sc_regs[30]
	Dump_Alpha()
#elif defined __sparc__
	#define REG_GET(ign, reg) &r->gregs[REG_##reg]
	Dump_SPARC()
#elif defined __PPC__ && __WORDSIZE == 32
	#define REG_GNUM(num)     &r->gregs[num]
	#define REG_GET_PC()      &r->gregs[32]
	#define REG_GET_LR()      &r->gregs[35]
	#define REG_GET_CTR()     &r->gregs[34]
	Dump_PPC()
#elif defined __PPC__
	#define REG_GNUM(num)     &r->gp_regs[num]
	#define REG_GET_PC()      &r->gp_regs[32]
	/* TODO this might be wrong, compare with PT_LNK in */
	/* https://elixir.bootlin.com/linux/v4.19.122/source/arch/powerpc/include/uapi/asm/ptrace.h#L102 */
	#define REG_GET_LR()      &r->gp_regs[35]
	#define REG_GET_CTR()     &r->gp_regs[34]
	Dump_PPC()
#elif defined __mips__
	#define REG_GNUM(num)     &r->gregs[num]
	#define REG_GET_PC()      &r->pc
	#define REG_GET_LO()      &r->mdlo
	#define REG_GET_HI()      &r->mdhi
	Dump_MIPS()
#elif defined __riscv
	#define REG_GNUM(num)     &r->__gregs[num]
	#define REG_GET_PC()      &r->__gregs[REG_PC]
	Dump_RISCV()
#elif defined __e2k__
	#define REG_GET_PSP()     &r->psp_hi  /* Procedure Stack Pointer */
	#define REG_GET_PCSP()    &r->pcsp_hi /* Procedure Chain Stack Pointer */
	#define REG_GET_SBR()     &r->sbr     /* User Stack Base Pointer */
	#define REG_GET_USD()     &r->usd_hi  /* User Stack Pointer */
	#define REG_GET_IP()      &r->cr0_hi  /* CR0 contains Instruction Pointer */
	Dump_E2K()
#else
	#error "Unknown CPU architecture"
#endif
}
