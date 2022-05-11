#include "types.h"
#include "param.h"
#include "aarch64.h"
#include "mmio.h"
#include "memmap.h"
#include "vcpu.h"
#include "vm.h"

struct mmio_info mmio_infos[128];

static struct mmio_info *alloc_mmio_info(struct mmio_info *prev) {
  for(struct mmio_info *m = mmio_infos; m < &mmio_infos[128]; m++) {
    if(m->size == 0) {
      m->size = 1;
      m->next = prev;
      return m;
    }
  }

  panic("nomem");
  return NULL;
}

int mmio_emulate(struct vcpu *vcpu, u64 *reg, struct mmio_access *mmio) {
  struct mmio_info *map = vcpu->vm->pmap;
  if(!map)
    return -1;

  u64 ipa = mmio->ipa;

  for(struct mmio_info *m = map; m->size != 0; m++) {
    if(m->base <= ipa && ipa < m->base + m->size) {
      if(mmio->wnr && m->write)
        return m->write(vcpu, ipa - m->base, *reg, mmio);
      else if(m->read)
        return m->read(vcpu, ipa - m->base, reg, mmio);
      else
        return -1;
    }
  }

  return -1;
}

int mmio_reg_handler(struct vm *vm, u64 ipa, u64 size,
                     int (*read)(struct vcpu *, u64, u64 *, struct mmio_access *),
                     int (*write)(struct vcpu *, u64, u64, struct mmio_access *)) {
  if(size == 0)
    return -1;

  struct mmio_info *new = alloc_mmio_info(vm->pmap);
  vm->pmap = new;

  new->base = ipa;
  new->size = size;
  new->read = read;
  new->write = write;

  return 0;
}
