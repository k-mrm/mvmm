#ifndef MVMM_SPINLOCK_H
#define MVMM_SPINLOCK_H

#include "aarch64.h"
#include "types.h"

// #define SPINLOCK_DEBUG

#ifdef SPINLOCK_DEBUG

struct spinlock {
  int cpuid;
  u8 lock;
};

typedef struct spinlock spinlock_t;

static inline int holdinglk(spinlock_t *lk) {
  if(lk->lock && lk->cpuid == cpuid())
    return 1;
  else
    return 0;
}

#else

typedef u8 spinlock_t;

#endif  /* SPINLOCK_DEBUG */

static inline void acquire(spinlock_t *lk) {
#ifdef SPINLOCK_DEBUG
  if(holdinglk(lk))
    panic("acquire: already held");

  asm volatile(
    "mov x1, %0\n"
    "mov w2, #1\n"
    "1: ldaxr w3, [x1]\n"
    "cbnz w3, 1b\n"
    "stxr w3, w2, [x1]\n"
    "cbnz w3, 1b\n"
    :: "r"(&lk->locked)
  );

  lk->cpuid = cpuid();
#else
  asm volatile(
    "mov x1, %0\n"
    "mov w2, #1\n"
    "1: ldaxr w3, [x1]\n"
    "cbnz w3, 1b\n"
    "stxr w3, w2, [x1]\n"
    "cbnz w3, 1b\n"
    :: "r"(lk)
  );
#endif

  isb();
}

static inline void release(spinlock_t *lk) {
#ifdef SPINLOCK_DEBUG
  if(!holdinglk(lk))
    panic("release: invalid");

  lk->cpuid = -1;
  asm volatile("str wzr, %0" : "=m"(lk->locked) :: "memory");
#else
  asm volatile("str wzr, [%0]" : "=r"(lk) :: "memory");
#endif

  isb();
}

static inline void spinlock_init(spinlock_t *lk) {
#ifdef SPINLOCK_DEBUG
  lk->cpuid = -1;
  lk->lock = 0;
#else
  *lk = 0;
#endif
}

#endif
