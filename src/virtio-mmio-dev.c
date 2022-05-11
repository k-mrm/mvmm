#include "mmio.h"
#include "virtio-mmio.h"
#include "log.h"
#include "mm.h"
#include "memmap.h"
#include "vm.h"

struct virtio_mmio_dev vtdev;

char virtqueue[PAGESIZE*2];

static int virtq_read(struct vcpu *vcpu, u64 offset, u64 *val, struct mmio_access *mmio) {
  return -1;
}

static int virtq_write(struct vcpu *vcpu, u64 offset, u64 val, struct mmio_access *mmio) {
  u32 desc_size = sizeof(struct virtq_desc) * vtdev.qnum;

  if(offset >= desc_size)
    goto passthrough;

  u64 descoff = offset % sizeof(struct virtq_desc); 

  if(descoff == offsetof(struct virtq_desc, addr)) {
    vmm_log("miiiiiii addr %p %p %p\n", val, ipa2pa(vcpu->vm->vttbr, val), mmio->accsize);
    val = ipa2pa(vcpu->vm->vttbr, val);
  } else if(descoff == offsetof(struct virtq_desc, len)) {
    vmm_log("mmiiiiii len %d\n", val);
  }

passthrough:
  switch(mmio->accsize) {
    case ACC_BYTE:        *(u8 *)&virtqueue[offset] = val; break;
    case ACC_HALFWORD:    *(u16 *)&virtqueue[offset] = val; break;
    case ACC_WORD:        *(u32 *)&virtqueue[offset] = val; break;
    case ACC_DOUBLEWORD:  *(u64 *)&virtqueue[offset] = val; break;
    default: panic("?");
  }

  return 0;
}

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
    case VIRTIO_MMIO_QUEUE_NUM:
      vtdev.qnum = val;
      vmm_log("queuenum %d\n", val);
      break;
    case VIRTIO_MMIO_QUEUE_PFN: {
      u64 pfn_ipa = val << 12;
      vmm_log("queuepfn %p -> %p\n", pfn_ipa, &virtqueue);
      pagetrap(vcpu->vm, pfn_ipa, 0x2000, virtq_read, virtq_write);
      val = (u64)&virtqueue;
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
