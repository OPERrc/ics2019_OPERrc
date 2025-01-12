#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include "rtl/rtl.h"

/* RTL pseudo instructions */

static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(dest, &reg_l(r)); return;
    case 1: rtl_host_lm(dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline void rtl_sr(int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(&reg_l(r), src1); return;
    case 1: rtl_host_sm(&reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(&reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline void rtl_push(const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  rtl_subi(&cpu.esp, &cpu.esp, 4);
	// cpu.esp -= 4;
  rtl_sm(&cpu.esp, src1, 4);
	// vaddr_write(cpu.esp, *src1, 4);
}

static inline void rtl_pop(rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  rtl_lm(dest, &cpu.esp, 4);
  // rtl_lr(dest, R_ESP, 4);
	// *dest = vaddr_read(cpu.esp, 4);
  rtl_addi(&cpu.esp, &cpu.esp, 4);
	// cpu.esp += 4;
}

static inline void rtl_is_sub_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  rtl_msb(&t0, src1, width);
  rtl_not(&t1, src2);
  rtl_msb(&t1, &t1, width);
  rtl_msb(dest, res, width);
  // t0 = (*src1 >> (width * 8 - 1)) & 1;
  // t1 = (~(*src2) >> (width * 8 - 1)) & 1;
  // *dest = (*res >> (width * 8 - 1)) & 1;
  // *dest = (t0 == t1 && t0 != *dest) ? 1 : 0;
  rtl_setrelop(RELOP_EQ, &t1, &t0, &t1);
  rtl_setrelop(RELOP_NE, dest, &t0, dest);
  rtl_and(dest, &t1, dest);
  /**dest = (*src1 >> (width * 8 - 1) == ~(*src2) >> (width * 8 - 1) 
    && *src1 >> (width * 8 - 1) != *res >> (width * 8 - 1)) ? 1 : 0;*/
}

static inline void rtl_is_sub_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 - src2)
  // *dest = *res > *src1 ? 1 : 0;
  rtl_setrelop(RELOP_GTU, dest, res, src1);
}

static inline void rtl_is_add_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  /*t0 = (*src1 >> (width * 8 - 1)) & 1;
  t1 = (*src2 >> (width * 8 - 1)) & 1;
  *dest = (*res >> (width * 8 - 1)) & 1;*/
  rtl_msb(&t0, src1, width);
  rtl_msb(&t1, src2, width);
  rtl_msb(dest, res, width);
  // *dest = (t0 == t1 && t0 != *dest) ? 1 : 0;
  rtl_setrelop(RELOP_EQ, &t1, &t0, &t1);
  rtl_setrelop(RELOP_NE, dest, &t0, dest);
  rtl_and(dest, &t1, dest);
  /**dest = (*src1 >> (width * 8 - 1) == *src2 >> (width * 8 - 1) 
    && *src2 >> (width * 8 - 1) != *res >> (width * 8 - 1)) ? 1 : 0;*/
}

static inline void rtl_is_add_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  // *dest = *res < *src1 ? 1 : 0;
  rtl_setrelop(RELOP_LTU, dest, res, src1);
}

#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    rtl_andi(&t0, src, 1); \
    cpu.f = t0; \
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    *dest = cpu.f; \
  }

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)

static inline void rtl_update_ZF(const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  rtl_shli(&t0, result, (4 - width) * 8);
  cpu.ZF = (t0 == 0) ? 1 : 0;
}

static inline void rtl_update_SF(const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  rtl_msb(&t0, result, width);
  cpu.SF = (t0 == 1) ? 1 : 0;
}

static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
