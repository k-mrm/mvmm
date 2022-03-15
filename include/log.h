#ifndef MVMM_LOG_H
#define MVMM_LOG_H

#include "printf.h"

#define vmm_log(...)  printf("[vmm]: " __VA_ARGS__)

#endif
