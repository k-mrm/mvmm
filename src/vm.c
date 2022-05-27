#include "types.h"
#include "vm.h"
#include "vcpu.h"
#include "mm.h"
#include "pmalloc.h"
#include "lib.h"
#include "memmap.h"
#include "printf.h"
#include "log.h"
#include "mmio.h"

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

void pagetrap(struct vm *vm, u64 ipa, u64 size,
              int (*read_handler)(struct vcpu *, u64, u64 *, struct mmio_access *),
              int (*write_handler)(struct vcpu *, u64, u64, struct mmio_access *)) {
  u64 *vttbr = vm->vttbr;

  if(pagewalk(vttbr, ipa, 0))
    pageunmap(vttbr, ipa, size);

  if(mmio_reg_handler(vm, ipa, size, read_handler, write_handler) < 0)
    panic("?");

  tlb_flush();
}

void new_vm(char *name, int ncpu, u64 img_start, u64 img_size, u64 entry, u64 allocated) {
  vmm_log("new vm `%s`\n", name);
  vmm_log("n vcpu: %d\n", ncpu);
  vmm_log("allocated ram: %d byte\n", allocated);
  vmm_log("img_start %p img_size %p byte\n", img_start, img_size);

  if(img_size > allocated)
    panic("img_size > allocated");
  if(allocated % PAGESIZE != 0)
    panic("invalid mem size");
  if(ncpu > VCPU_MAX)
    panic("too many vcpu");

  struct vm *vm = allocvm();

  strcpy(vm->name, name);

  /* cpu0 */
  vm->vcpus[0] = new_vcpu(vm, 0, entry);

  /* cpuN */
  for(int i = 1; i < ncpu; i++)
    vm->vcpus[i] = new_vcpu(vm, i, 0);

  vm->nvcpu = ncpu;

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
    pagemap(vttbr, entry+p, (u64)page, PAGESIZE, S2PTE_NORMAL|S2PTE_RW);
  }

  for(; p < allocated; p += PAGESIZE) {
    char *page = pmalloc();
    if(!page)
      panic("ram");

    pagemap(vttbr, entry+p, (u64)page, PAGESIZE, S2PTE_NORMAL|S2PTE_RW);
  }

  /* map peripheral */
  pagemap(vttbr, UARTBASE, UARTBASE, PAGESIZE, S2PTE_DEVICE|S2PTE_RW);

  vm->vttbr = vttbr;
  vm->pmap = NULL;
  vm->vgic = new_vgic(vm);

  virtio_mmio_init(vm);

  spinlock_init(&vm->lock);

  vcpu_ready(vm->vcpus[0]);
}
