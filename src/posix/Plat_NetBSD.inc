/* See /usr/include/[ARCH]/mcontext.h */
/* -> src/sys/arch/[ARCH]/include/mcontext.h */
#include <signal.h>
#include <sys/ucontext.h>

void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	mcontext_t* r = &((ucontext_t*)ctx)->uc_mcontext;
#if defined __i386__
	#define REG_GET(ign, reg) &r->__gregs[_REG_E##reg]
	Dump_X86()
#elif defined __x86_64__
	#define REG_GET(ign, reg) &r->__gregs[_REG_R##reg]
	Dump_X64()
#elif defined __aarch64__
	#define REG_GNUM(num)     &r->__gregs[num]
	#define REG_GET_FP()      &r->__gregs[_REG_FP]
	#define REG_GET_LR()      &r->__gregs[_REG_LR]
	#define REG_GET_SP()      &r->__gregs[_REG_SP]
	#define REG_GET_PC()      &r->__gregs[_REG_PC]
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
	#define REG_GNUM(num)     &r->__gregs[num]
	#define REG_GET_PC()      &r->__gregs[_REG_PC]
	#define REG_GET_LR()      &r->__gregs[_REG_LR]
	#define REG_GET_CTR()     &r->__gregs[_REG_CTR]
	Dump_PPC()
#elif defined __sparc__
	#define REG_GET(ign, reg) &r->__gregs[_REG_##reg]
	Dump_SPARC()
#elif defined __mips__
	#define REG_GNUM(num)     &r->__gregs[num]
	#define REG_GET_PC()      &r->__gregs[_REG_EPC]
	#define REG_GET_LO()      &r->__gregs[_REG_MDLO]
	#define REG_GET_HI()      &r->__gregs[_REG_MDHI]
	Dump_MIPS()
#elif defined __riscv
	#define REG_GNUM(num)     &r->__gregs[num]
	#define REG_GET_PC()      &r->__gregs[_REG_PC]
	Dump_RISCV()
#else
	#error "Unknown CPU architecture"
#endif
}

