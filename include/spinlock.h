#ifndef MVMM_SPINLOCK_H
#define MVMM_SPINLOCK_H

#include "aarch64.h"
#include "types.h"

typedef u8 spinlock_t;

static inline void acquire(spinlock_t *lk) {
  asm volatile(
    "mov x1, %0\n"
    "mov w2, #1\n"
    "1: ldaxr w3, [x1]\n"
    "cbnz w3, 1b\n"
    "stxr w3, w2, [x1]\n"
    "cbnz w3, 1b\n"
    :: "r"(lk)
  );

  isb();
}

static inline void release(spinlock_t *lk) {
  asm volatile("str wzr, [%0]" : "=m"(lk) :: "memory");
}

static inline void spinlock_init(spinlock_t *lk) {
  *lk = 0;
}

#endif
