PREFIX = aarch64-linux-gnu-
CC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy

CPU = cortex-a72
QCPU = cortex-a72

CFLAGS = -Wall -Og -g -MD -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=$(CPU)
CFLAGS += -I ./include/
LDFLAGS = -nostdlib -nostartfiles

QEMUPREFIX =
QEMU = $(QEMUPREFIX)qemu-system-aarch64
GIC_VERSION = 3
MACHINE = virt,gic-version=$(GIC_VERSION),virtualization=on
ifndef NCPU
NCPU = 4
endif

OBJS = src/boot.o

QEMUOPTS = -cpu $(QCPU) -machine $(MACHINE) -smp $(NCPU) -m 256
QEMUOPTS += -global virtio-mmio.force-legacy=false
QEMUOPTS += -nographic -kernel mvmm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

-include: *.d

mvmm: $(OBJS) src/memory.ld
	$(LD) $(LDFLAGS) -T src/memory.ld -o $@ $(OBJS)

qemu: mvmm
	$(QEMU) $(QEMUOPTS)

gdb: mvmm
	$(QEMU) -S -gdb tcp::1234 $(QEMUOPTS)

clean:
	$(RM) $(OBJS) mvmm

.PHONY: qemu gdb clean
