/* See CONTEXT in WinNT.h */
void CrashHandler_DumpRegisters(void* ctx, cc_string* str) {
	CONTEXT* r = (CONTEXT*)ctx;
#if defined _M_IX86
	#define REG_GET(reg, ign) &r->E ## reg
	Dump_X86()
#elif defined _M_X64
	#define REG_GET(reg, ign) &r->R ## reg
	Dump_X64()
#elif defined _M_ARM64
	#define REG_GNUM(num)     &r->X[num]
	#define REG_GET_FP()      &r->Fp
	#define REG_GET_LR()      &r->Lr
	#define REG_GET_SP()      &r->Sp
	#define REG_GET_PC()      &r->Pc
	Dump_ARM64()
#elif defined _M_ARM
	#define REG_GNUM(num)     &r->R ## num
	#define REG_GET_FP()      &r->R11
	#define REG_GET_IP()      &r->R12
	#define REG_GET_SP()      &r->Sp
	#define REG_GET_LR()      &r->Lr
	#define REG_GET_PC()      &r->Pc
	Dump_ARM32()
#elif defined _M_PPC
	#define REG_GNUM(num) &r->Gpr ## num
	#define REG_GET_PC()  &r->Iar
	#define REG_GET_LR()  &r->Lr
	#define REG_GET_CTR() &r->Ctr
	Dump_PPC()
#elif defined _M_ALPHA
	#define REG_GNUM(num) &r->IntV0 + num
	#define REG_GET_FP()  &r->IntFp
	#define REG_GET_SP()  &r->IntSp
	#define REG_GET_PC()  &r->Fir
	Dump_Alpha()
#elif defined _MIPS_
	#define REG_GNUM(num) &r->IntZero + num
	#define REG_GET_PC()  &r->Fir
	#define REG_GET_LO()  &r->IntLo
	#define REG_GET_HI()  &r->IntHi
	Dump_MIPS()
#elif defined SHx
	#define REG_GNUM(num)  &r->R ## num
	#define REG_GET_MACL() &r->MACL
	#define REG_GET_MACH() &r->MACH
	#define REG_GET_GBR()  &r->GBR
	#define REG_GET_PC()   &r->Fir
	#define REG_GET_RA()   &r->PR
	Dump_SH()
#else
	#error "Unknown CPU architecture"
#endif
}
