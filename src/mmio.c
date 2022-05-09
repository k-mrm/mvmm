#include "types.h"
#include "param.h"
#include "aarch64.h"
#include "mmio.h"
#include "memmap.h"
#include "vcpu.h"

int vgicd_mmio_read(struct vcpu *vcpu, u64 offset, u64 *val, struct mmio_access *mmio);
int vgicd_mmio_write(struct vcpu *vcpu, u64 offset, u64 val, struct mmio_access *mmio);
int vgicr_mmio_read(struct vcpu *vcpu, u64 offset, u64 *val, struct mmio_access *mmio);
int vgicr_mmio_write(struct vcpu *vcpu, u64 offset, u64 val, struct mmio_access *mmio);
int vtdev_mmio_read(struct vcpu *vcpu, u64 offset, u64 *val, struct mmio_access *mmio);
int vtdev_mmio_write(struct vcpu *vcpu, u64 offset, u64 *val, struct mmio_access *mmio);

struct mmio_info virtmap[] = {
  {GICDBASE, 0x10000, vgicd_mmio_read, vgicd_mmio_write},
  {GICRBASE, 0xf60000, vgicr_mmio_read, vgicr_mmio_write},
  {VIRTIO0, 0x200, vtdev_mmio_read, vtdev_mmio_write},
  {0,0,NULL,NULL},
};

int mmio_emulate(struct vcpu *vcpu, u64 *reg, struct mmio_access *mmio) {
  struct mmio_info *map = vcpu->vm->pmap;
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
