#include "types.h"
#include "param.h"
#include "aarch64.h"
#include "mmio.h"
#include "memmap.h"
#include "vcpu.h"

int vgicd_mmio_read(struct vcpu *vcpu, u64 offset, u64 *val, enum mmio_accsize accsize);
int vgicd_mmio_write(struct vcpu *vcpu, u64 offset, u64 val, enum mmio_accsize accsize);
int vgicr_mmio_read(struct vcpu *vcpu, u64 offset, u64 *val, enum mmio_accsize accsize);
int vgicr_mmio_write(struct vcpu *vcpu, u64 offset, u64 val, enum mmio_accsize accsize);

struct mmio_info virtmap[] = {
  {GICDBASE, 0x10000, vgicd_mmio_read, vgicd_mmio_write},
  {GICRBASE, 0xf60000, vgicr_mmio_read, vgicr_mmio_write},
  {0,0,NULL,NULL},
};

int mmio_emulate(struct vcpu *vcpu, u64 ipa, u64 *reg, enum mmio_accsize accsize, bool wr) {
  struct mmio_info *map = vcpu->vm->pmap;

  for(struct mmio_info *m = map; m->size != 0; m++) {
    if(m->base <= ipa && ipa < m->base + m->size) {
      if(wr && m->write)
        return m->write(vcpu, ipa - m->base, *reg, accsize);
      else if(m->read)
        return m->read(vcpu, ipa - m->base, reg, accsize);
      else
        return -1;
    }
  }

  return -1;
}
