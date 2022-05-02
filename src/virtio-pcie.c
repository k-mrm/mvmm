#include "pcie.h"
#include "virtio-pcie.h"
#include "log.h"

static void virtio_net_init() {
  vmm_log("virtio_net_init\n");
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

static void virtio_rng_init() {
  vmm_log("virtio_rng_init\n");
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
      virtio_rng_init();
      break;
    default:
      return -1;
  }

  return 0;
}
