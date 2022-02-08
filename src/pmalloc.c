#include "types.h"
#include "lib.h"
#include "mm.h"
#include "memmap.h"

struct header {
  struct header *next;
};

struct header *freelist;

extern char vmm_end[];

void *pmalloc() {
  struct header *new = freelist;
  if(!new) { /* no memory or uninitialized */
    return NULL;
  }

  freelist = new->next;

  memset((char *)new, 0, PAGESIZE);

  return (void *)new;
}

void pfree(void *p) {
  if(p == NULL)
    return;

  memset(p, 0, PAGESIZE);

  struct header *fp = (struct header *)p;

  fp->next = freelist;
  freelist = fp;
}

void pmalloc_init() {
  for(u64 s = (u64)vmm_end; s < PHYEND; s += PAGESIZE)
    pfree((void *)s);
}
