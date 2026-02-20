/* See /usr/include/sys/ucontext.h */
/* https://nixdoc.net/man-pages/IRIX/man5/UCONTEXT.5.html */
#include <signal.h>
#include <sys/ucontext.h>

void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	mcontext_t* r = &((ucontext_t*)ctx)->uc_mcontext;

	#define REG_GNUM(num) &r->__gregs[CTX_EPC]
	#define REG_GET_PC()  &r->__gregs[CTX_MDLO]
	#define REG_GET_LO()  &r->__gregs[CTX_MDLO]
	#define REG_GET_HI()  &r->__gregs[CTX_MDHI]
	Dump_MIPS()
}
