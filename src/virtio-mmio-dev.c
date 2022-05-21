#include "mmio.h"
#include "virtio-mmio.h"
#include "log.h"
#include "mm.h"
#include "memmap.h"
#include "vm.h"
#include "pmalloc.h"

struct virtio_mmio_dev vtdev = {0};

__attribute__((aligned(PAGESIZE)))
char virtqueue[PAGESIZE*2] = {0};

static int virtq_read(struct vcpu *vcpu, u64 offset, u64 *val, struct mmio_access *mmio) {
  u32 desc_size = sizeof(struct virtq_desc) * vtdev.qnum;

  if(offset >= desc_size)
    goto passthrough;

  u64 descoff = offset % sizeof(struct virtq_desc); 

passthrough:
  switch(mmio->accsize) {
    case ACC_BYTE:        *val = *(u8 *)(&virtqueue[offset]); break;
    case ACC_HALFWORD:    *val = *(u16 *)(&virtqueue[offset]); break;
    case ACC_WORD:        *val = *(u32 *)(&virtqueue[offset]); break;
    case ACC_DOUBLEWORD:  *val = *(u64 *)(&virtqueue[offset]); break;
    default: panic("?");
  }

  return 0;
}

static int virtq_write(struct vcpu *vcpu, u64 offset, u64 val, struct mmio_access *mmio) {
  u32 desc_size = sizeof(struct virtq_desc) * vtdev.qnum;

  if(offset >= desc_size)
    goto passthrough;

  u64 descoff = offset % sizeof(struct virtq_desc); 
  u64 descn = offset / sizeof(struct virtq_desc); 

  switch(descoff) {
    case offsetof(struct virtq_desc, addr):
      if(val) {
        // vmm_log("miiiiiii addr %p %p %p\n", val, ipa2pa(vcpu->vm->vttbr, val), mmio->accsize);
        vtdev.ring[descn].ipa = val;
        val = ipa2pa(vcpu->vm->vttbr, val);
        vtdev.ring[descn].real_addr = val;
      } else {
        vtdev.ring[descn].ipa = 0;
        vtdev.ring[descn].real_addr = 0;
      }
      break;
    case offsetof(struct virtq_desc, len): {
      u32 len = (u32)val;
      vtdev.ring[descn].len = len;
      vtdev.ring[descn].across_page = false;

      u64 daddr = vtdev.ring[descn].real_addr;
      /* check acrossing pages */
      if(((daddr+len)>>12) > (daddr>>12)) {
        char *real = pmalloc();
        copy_from_guest(vcpu->vm->vttbr, real, vtdev.ring[descn].ipa, len);
        vtdev.ring[descn].real_addr = (u64)real;
        vtdev.ring[descn].across_page = true;

        /* rewrite virtqueue */
        u64 daddr_offset =
          descn * sizeof(struct virtq_desc) + offsetof(struct virtq_desc, addr);
        *(u64 *)(&virtqueue[daddr_offset]) = (u64)real;
      }
      break;
    }
    case offsetof(struct virtq_desc, flags): {
      u16 flags = (u16)val;
      if(!(flags & VRING_DESC_F_NEXT))
        vtdev.ring[descn].next = NULL;
      break;
    }
    case offsetof(struct virtq_desc, next): {
      u16 next = (u16)val;
      vtdev.ring[descn].next = &vtdev.ring[next];
      break;
    }
  }

passthrough:
  switch(mmio->accsize) {
    case ACC_BYTE:        *(u8 *)(&virtqueue[offset]) = val; break;
    case ACC_HALFWORD:    *(u16 *)(&virtqueue[offset]) = val; break;
    case ACC_WORD:        *(u32 *)(&virtqueue[offset]) = val; break;
    case ACC_DOUBLEWORD:  *(u64 *)(&virtqueue[offset]) = val; break;
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
    case VIRTIO_MMIO_GUEST_PAGE_SIZE:
      if(val != PAGESIZE)
        panic("unsupported");
      vmm_log("guest pagesize: %d\n", val);
      break;
    case VIRTIO_MMIO_QUEUE_NOTIFY:
      // vmm_log("queue notify val: %d\n", val);
      break;
    case VIRTIO_MMIO_QUEUE_PFN: {
      u64 pfn_ipa = val << 12;
      pagetrap(vcpu->vm, pfn_ipa, 0x2000, virtq_read, virtq_write);
      val = (u64)virtqueue >> 12;
      vmm_log("queuepfn %p -> %p(%p)\n", pfn_ipa, virtqueue, val);
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

void virtio_dev_intr(struct vcpu *vcpu) {
  struct virtq_used *used = (struct virtq_used *)(virtqueue + PAGESIZE);
  vmm_log("usedring %p %d\n", used, used->idx);

  while(vtdev.last_used_idx != used->idx) {
    __sync_synchronize();
    int id = used->ring[vtdev.last_used_idx % NUM].id;
    struct vtdev_desc *d = &vtdev.ring[id];
    for(; d; d = d->next)
      if(d->across_page) {
        vmm_log("hello\n");
        copy_to_guest(vcpu->vm->vttbr, d->ipa, (char *)d->real_addr, d->len);

        pfree((char *)d->real_addr);
        d->across_page = false;
      }

    vtdev.last_used_idx++;
  }
}

void virtio_mmio_init(struct vm *vm) {
  pagetrap(vm, VIRTIO0, 0x1000, virtio_mmio_read, virtio_mmio_write);
}
