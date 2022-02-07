#include "types.h"
#include "lib.h"
#include "mm.h"

struct header {
  struct header *next;
};

struct header *freelist;

extern char __vmm_alloc_start[];
extern char __vmm_alloc_end[];

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
  u64 s = (u64)__vmm_alloc_start;
  u64 e = (u64)__vmm_alloc_end;

  for(; s < e; s += PAGESIZE)
    pfree((void *)s);
}
