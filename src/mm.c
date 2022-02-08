#include "mm.h"
#include "lib.h"
#include "pmalloc.h"

static u64 *pagewalk(u64 *pgt, u64 va) {
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

static void pagemap(u64 *pgt, u64 va, u64 pa, u64 size, u64 attr) {
  if(va % PAGESIZE != 0 || pa % PAGESIZE != 0 || size % PAGESIZE != 0)
    panic("invalid pagemap");

  for(u64 p = 0; p < size; p += PAGESIZE, va += PAGESIZE, pa += PAGESIZE) {
    u64 *pte = pagewalk(pgt, va);
    if(*pte & PTE_AF)
      panic("this entry has been used");

    *pte = PTE_PA(pa) | PTE_AF | attr | PTE_V;
  }
}

static void pageunmap(u64 *pgt, u64 va, u64 size) {
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

