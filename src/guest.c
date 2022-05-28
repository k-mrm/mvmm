#include "types.h"
#include "guest.h"

extern char _binary_guest_xv6_kernel_img_start[];
extern char _binary_guest_xv6_kernel_img_size[];
extern char _binary_virt_dtb_start[];
extern char _binary_virt_dtb_size[];

struct guest hello = {
  .start = (u64)_binary_guest_xv6_kernel_img_start,
  .size = (u64)_binary_guest_xv6_kernel_img_size,
};

struct guest virt_dtb = {
  .start = (u64)_binary_virt_dtb_start,
  .size = (u64)_binary_virt_dtb_size,
};

#if 0

extern char _binary_guest_hello_hello_img_start[];
extern char _binary_guest_hello_hello_img_size[];

struct guest hello = {
  .start = (u64)_binary_guest_hello_hello_img_start,
  .size = (u64)_binary_guest_hello_hello_img_size,
};

#endif
