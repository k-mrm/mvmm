#include "types.h"
#include "lib.h"
#include "mm.h"
#include "memmap.h"
#include "spinlock.h"

struct header {
  struct header *next;
};

extern char vmm_end[];

struct {
  spinlock_t lock;
  struct header *freelist;
} pmem;

void *pmalloc() {
  acquire(&pmem.lock);

  struct header *new = pmem.freelist;
  if(!new) { /* no memory or uninitialized */
    return NULL;
  }

  pmem.freelist = new->next;

  release(&pmem.lock);

  memset((char *)new, 0, PAGESIZE);

  return (void *)new;
}

void pfree(void *p) {
  if(p == NULL)
    return;

  memset(p, 0, PAGESIZE);

  struct header *fp = (struct header *)p;

  acquire(&pmem.lock);
  fp->next = pmem.freelist;
  pmem.freelist = fp;
  release(&pmem.lock);
}

void pmalloc_init() {
  spinlock_init(&pmem.lock);
  for(u64 s = (u64)vmm_end; s < PHYEND; s += PAGESIZE)
    pfree((void *)s);
}
