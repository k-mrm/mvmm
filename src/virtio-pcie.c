#include "pcie.h"
#include "virtio-pcie.h"
#include "log.h"
#include "pmalloc.h"

static void desc_init(struct virtq *vq) {
  for(int i = 0; i < NQUEUE; i++) {
    if(i != NQUEUE - 1) {
      vq->desc[i].flags = VIRTQ_DESC_F_NEXT;
      vq->desc[i].next = i + 1;
    }
  }
}

static void virtq_init(struct virtq *vq) {
  vq->desc = pmalloc();
  vq->avail = pmalloc();
  vq->used = pmalloc();

  vq->nfree = NQUEUE;

  desc_init(vq);
}

static int virtio_net_init(struct virtio_pci_dev *vdev) {
  vmm_log("virtio_net_init\n");
  return -1;
}

static int virtio_rng_init(struct virtio_pci_dev *vdev) {
  vmm_log("virtio_rng_init\n");
  struct virtio_pci_common_cfg *vtcfg = vdev->vtcfg;

  vtcfg->device_status = 0;

  u8 status = DEV_STATUS_ACKNOWLEDGE;
  vtcfg->device_status = status;
  isb();

  status |= DEV_STATUS_DRIVER;
  vtcfg->device_status = status;
  isb();

  vtcfg->device_feature_select = 0;
  vtcfg->driver_feature_select = 0;

  status |= DEV_STATUS_FEATURES_OK;
  vtcfg->device_status = status;
  isb();

  virtq_init(&vdev->virtq);

  vtcfg->queue_select = 0;
  vtcfg->queue_size = 1; 

  vtcfg->queue_desc = (u64)vdev->virtq.desc;
  vtcfg->queue_driver = (u64)vdev->virtq.avail;
  vtcfg->queue_device = (u64)vdev->virtq.used;
  isb();

  vtcfg->queue_enable = 1;
  isb();

  status |= DEV_STATUS_DRIVER_OK;
  vtcfg->device_status = status;
  isb();

  return -1;
}

static void __virtio_scan_cap(struct virtio_pci_dev *vdev, struct virtio_pci_cap *cap) {
  if(cap->cap_vndr != 0x9)
    vmm_warn("virtio-pci invalid vendor %p\n", cap->cap_vndr);

  vmm_log("cap vndr %p next %p\n", cap->cap_vndr, cap->cap_next);

  struct pci_dev *pdev = vdev->pci;

  switch(cap->cfg_type) {
    case VIRTIO_PCI_CAP_COMMON_CFG: {
      u64 addr = pdev->reg_addr[cap->bar];
      struct virtio_pci_common_cfg *vtcfg = (struct virtio_pci_common_cfg *)addr;

      vmm_log("vtcfg %p %d %d %p\n", vtcfg, vtcfg->num_queues, vtcfg->queue_enable, vtcfg->device_status);

      vdev->vtcfg = vtcfg;

      break;
    }
    case VIRTIO_PCI_CAP_NOTIFY_CFG:
    case VIRTIO_PCI_CAP_ISR_CFG:
    case VIRTIO_PCI_CAP_DEVICE_CFG:
    case VIRTIO_PCI_CAP_PCI_CFG:
    default:
      break;
  }

  if(cap->cap_next) {
    cap = (struct virtio_pci_cap *)((char *)(pdev->cfg) + cap->cap_next);
    __virtio_scan_cap(vdev, cap);
  }
}

static void virtio_scan_cap(struct virtio_pci_dev *vdev) {
  struct pci_config *cfg = vdev->pci->cfg;
  struct virtio_pci_cap *cap = (struct virtio_pci_cap *)((char *)cfg + cfg->cap_ptr);

  __virtio_scan_cap(vdev, cap);
}

int virtio_pci_dev_init(struct pci_dev *pci_dev) {
  if(pci_dev->dev_id < 0x1040)
    return -1;

  struct virtio_pci_dev vdev;
  vdev.pci = pci_dev;
  virtio_scan_cap(&vdev);

  switch(pci_dev->dev_id - 0x1040) {
    case 1:
      virtio_net_init(&vdev);
      break;
    case 4:
      virtio_rng_init(&vdev);
      break;
    default:
      return -1;
  }

  return 0;
}
