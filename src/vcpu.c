#include "vcpu.h"
#include "printf.h"

struct vcpu vcpus[VCPU_MAX];

struct vcpu *allocvcpu() {
  for(struct vcpu *vcpu = vcpus; vcpu < &vcpus[VCPU_MAX]; vcpu++) {
    if(vcpu->state == UNUSED) {
      vcpu->state = CREATED;
      return vcpu;
    }
  }

  return NULL;
}

void schedule() {
  for(;;) {
    for(struct vcpu *vcpu = vcpus; vcpu < &vcpus[VCPU_MAX]; vcpu++) {
      if(vcpu->state == READY) {
        printf("vcpu ready!\n");
      }
    }
  }
}
