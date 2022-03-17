#include "types.h"
#include "vm.h"
#include "vcpu.h"
#include "mm.h"
#include "pmalloc.h"
#include "lib.h"
#include "memmap.h"
#include "printf.h"
#include "log.h"

struct vm vms[VM_MAX];

static struct vm *allocvm() {
  for(struct vm *vm = vms; vm < &vms[VM_MAX]; vm++) {
    if(vm->used == 0) {
      vm->used = 1;
      return vm;
    }
  }

  return NULL;
}

void new_vm(char *name, int ncpu, u64 img_start, u64 img_size, u64 entry, u64 allocated) {
  vmm_log("new vm `%s`\n", name);
  vmm_log("n vcpu: %d\n", ncpu);
  vmm_log("allocated ram: %d byte\n", allocated);

  if(img_size > allocated)
    panic("img_size > allocated");
  if(allocated % PAGESIZE != 0)
    panic("mem align");

  struct vm *vm = allocvm();

  strcpy(vm->name, name);
  struct vcpu *vtmp[ncpu];

  for(int i = 0; i < ncpu; i++)
    vtmp[i] = new_vcpu(vm, i, entry);

  vm->entry = entry;

  u64 *vttbr = pmalloc();
  if(!vttbr)
    panic("vttbr");

  u64 p, cpsize;
  for(p = 0; p < img_size; p += PAGESIZE) {
    char *page = pmalloc();
    if(!page)
      panic("img");

    if(img_size - p > PAGESIZE)
      cpsize = PAGESIZE;
    else
      cpsize = img_size - p;

    memcpy(page, (char *)img_start+p, cpsize);
    pagemap(vttbr, entry+p, (u64)page, PAGESIZE, S2PTE_NORMAL);
  }

  for(; p < allocated; p += PAGESIZE) {
    char *page = pmalloc();
    if(!page)
      panic("ram");

    pagemap(vttbr, entry+p, (u64)page, PAGESIZE, S2PTE_NORMAL);
  }

  /* map peripheral */
  pagemap(vttbr, UARTBASE, UARTBASE, PAGESIZE, S2PTE_DEVICE);
  // pagemap(vttbr, GICDBASE, GICDBASE, 0x10000, S2PTE_DEVICE);
  // pagemap(vttbr, GICRBASE, GICRBASE, 0xf60000, S2PTE_DEVICE);

  vm->vttbr = vttbr;

  vm->vgic = new_vgic();

  vm->pmap = virtmap;

  for(int i = 0; i < ncpu; i++)
    vtmp[i]->state = READY;
}
