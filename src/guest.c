#include "types.h"

extern char _binary_guest_hello_img_start[];
extern char _binary_guest_hello_img_size[];

struct guest {
  u64 start;
  u64 size;
};

struct guest hello = {
  .start = (u64)_binary_guest_hello_img_start,
  .size = (u64)_binary_guest_hello_img_size,
};
