#include "mmio.h"
#include "virtio-mmio.h"

int vtdev_mmio_read(struct vcpu *vcpu, u64 offset, u64 *val, struct mmio_access *mmio) {
  return -1;
}

int vtdev_mmio_write(struct vcpu *vcpu, u64 offset, u64 val, struct mmio_access *mmio) {
  return -1;
}
