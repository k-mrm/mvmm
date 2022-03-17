#include "types.h"
#include "param.h"
#include "aarch64.h"
#include "mmio.h"
#include "memmap.h"

struct mmio_info virtmap[] = {
  {GICDBASE, 0x10000, NULL, NULL},
  {GICRBASE, 0xf60000, NULL, NULL},
  {0,0,NULL,NULL},
};

int mmio_emulate(struct vcpu *vcpu, int reg, u64 ipa, int accsize, bool wr) {
  struct mmio_info *map = vcpu->vm->pmap;

  for(struct mmio_info *m = map; m->size != 0; m++) {
    if(m->base <= ipa && ipa < m->base + m->size) {
      if(wr && m->write)
        return m->write(vcpu, reg, ipa - m->base, accsize);
      else if(m->read)
        return m->read(vcpu, reg, ipa - m->base, accsize);
      else
        return -1;
    }
  }
}

static int mmio_read() {
  ;
}

static int mmio_write() {
  ;
}
