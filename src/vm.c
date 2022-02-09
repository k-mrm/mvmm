#include "types.h"
#include "vm.h"
#include "vcpu.h"
#include "mm.h"
#include "pmalloc.h"
#include "lib.h"
#include "printf.h"

struct vm vms[VM_MAX];

static struct vm *allocvm() {
  for(struct vm *vm = &vms[0]; vm < &vms[VM_MAX]; vm++) {
    if(vm->used == 0) {
      vm->used = 1;
      return vm;
    }
  }

  return NULL;
}

void new_vm(int ncpu, u64 img_start, u64 img_size, u64 entry, u64 allocated) {
  struct vm *vm = allocvm();

  for(int i = 0; i < ncpu; i++) {
    struct vcpu *vcpu = allocvcpu();
    if(!vcpu)
      panic("vcpu");
    vcpu->vm = vm;
  }

  u64 *vttbr = pmalloc();
  if(!vttbr)
    panic("vttbr");

  u64 p;
  for(p = 0; p < img_size; p += PAGESIZE) {
    /* TODO: img_size > PAGESIZE */
    char *page = pmalloc();
    if(!page)
      panic("img");
    memcpy(page, (char *)img_start, img_size);
    pagemap(vttbr, entry+p, (u64)page, PAGESIZE, PTE_NORMAL);
  }

  if(allocated % PAGESIZE != 0)
    panic("mem align");
  for(; p < allocated; p += PAGESIZE) {
    char *page = pmalloc();
    if(!page)
      panic("ram");
    pagemap(vttbr, entry+p, (u64)page, PAGESIZE, PTE_NORMAL);
  }

  vm->vttbr = vttbr;
}
