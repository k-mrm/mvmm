#include "mm.h"
#include "aarch64.h"
#include "lib.h"
#include "pmalloc.h"
#include "printf.h"

u64 *pagewalk(u64 *pgt, u64 va) {
  for(int level = 1; level < 3; level++) {
    u64 *pte = &pgt[PIDX(level, va)];

    if((*pte & PTE_VALID) && (*pte & PTE_TABLE)) {
      pgt = (u64 *)PTE_PA(*pte);
    } else {
      pgt = pmalloc();
      if(!pgt)
        return NULL;

      *pte = PTE_PA(pgt) | PTE_TABLE | PTE_VALID;
    }
  }

  return &pgt[PIDX(3, va)];
}

void pagemap(u64 *pgt, u64 va, u64 pa, u64 size, u64 attr) {
  if(va % PAGESIZE != 0 || pa % PAGESIZE != 0 || size % PAGESIZE != 0)
    panic("invalid pagemap");

  for(u64 p = 0; p < size; p += PAGESIZE, va += PAGESIZE, pa += PAGESIZE) {
    u64 *pte = pagewalk(pgt, va);
    if(*pte & PTE_AF)
      panic("this entry has been used");

    *pte = PTE_PA(pa) | S2PTE_AF | attr | PTE_V;
  }
}

void pageunmap(u64 *pgt, u64 va, u64 size) {
  if(va % PAGESIZE != 0 || size % PAGESIZE != 0)
    panic("invalid pageunmap");

  for(u64 p = 0; p < size; p += PAGESIZE, va += PAGESIZE) {
    u64 *pte = pagewalk(pgt, va);
    if(*pte == 0)
      panic("bad unmap");

    u64 pa = PTE_PA(*pte);
    pfree((void *)pa);
    *pte = 0;
  }
}

u64 ipa2pa(u64 *pgt, u64 ipa) {
  u64 *pte = pagewalk(pgt, ipa);
  return PTE_PA(*pte);
}

void s2mmu_init(void) {
  u64 vtcr = VTCR_T0SZ(25) | VTCR_SH0(0) | VTCR_SL0(1) |
             VTCR_TG0(0) | VTCR_NSW | VTCR_NSA | VTCR_PS(2);
  write_sysreg(vtcr_el2, vtcr);

  u64 mair = (AI_DEVICE_nGnRnE << (8 * AI_DEVICE_nGnRnE_IDX)) | (AI_NORMAL_NC << (8 * AI_NORMAL_NC_IDX));
  write_sysreg(mair_el2, mair);

  isb();
}
