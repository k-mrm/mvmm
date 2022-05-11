#include "mmio.h"
#include "virtio-mmio.h"
#include "log.h"
#include "mm.h"
#include "memmap.h"
#include "vm.h"

/* legacy */
static int virtio_mmio_read(struct vcpu *vcpu, u64 offset, u64 *val, struct mmio_access *mmio) {
  volatile void *ipa = (void *)mmio->ipa;

  switch(mmio->accsize) {
    case ACC_BYTE:        *val = *(u8 *)ipa; break;
    case ACC_HALFWORD:    *val = *(u16 *)ipa; break;
    case ACC_WORD:        *val = *(u32 *)ipa; break;
    case ACC_DOUBLEWORD:  *val = *(u64 *)ipa; break;
    default: panic("?");
  }

  return 0;
}

/* legacy */
static int virtio_mmio_write(struct vcpu *vcpu, u64 offset, u64 val, struct mmio_access *mmio) {
  volatile void *ipa = (void *)mmio->ipa;

  switch(offset) {
    case VIRTIO_MMIO_QUEUE_PFN: {
      u64 pfn_ipa = val << 12;
      u64 pfn_pa = ipa2pa(vcpu->vm->vttbr, pfn_ipa);
      vmm_log("queuepfn %p -> %p\n", pfn_ipa, pfn_pa);
      pagetrap(vcpu->vm, pfn_ipa, 0x1000, NULL, NULL);
      val = pfn_pa;
      break;
    }
  }

  switch(mmio->accsize) {
    case ACC_BYTE:        *(u8 *)ipa = val; break;
    case ACC_HALFWORD:    *(u16 *)ipa = val; break;
    case ACC_WORD:        *(u32 *)ipa = val; break;
    case ACC_DOUBLEWORD:  *(u64 *)ipa = val; break;
    default: panic("?");
  }

  return 0;
}

void virtio_mmio_init(struct vm *vm) {
  pagetrap(vm, VIRTIO0, 0x1000, virtio_mmio_read, virtio_mmio_write);
}
