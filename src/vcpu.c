#include "vcpu.h"

struct vcpu vcpus[VCPU_MAX];

struct vcpu *allocvcpu() {
  for(struct vcpu *vcpu = &vcpus[0]; vcpu < &vcpus[VCPU_MAX]; vcpu++) {
    if(vcpu->state == UNUSED) {
      vcpu->state = CREATED;
      return vcpu;
    }
  }

  return NULL;
}
