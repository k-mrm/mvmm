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
NCPU = 1
endif

OBJS = src/boot.o src/init.o src/uart.o src/lib.o src/pmalloc.o src/printf.o src/vcpu.o \
			 src/vm.o src/mm.o src/vector.o src/guest.o src/trap.o src/pcpu.o src/vgic.o

QEMUOPTS = -cpu $(QCPU) -machine $(MACHINE) -smp $(NCPU) -m 256
QEMUOPTS += -global virtio-mmio.force-legacy=false
QEMUOPTS += -nographic -kernel mvmm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

guest/hello.img: guest/Makefile
	make -C guest

-include: *.d

mvmm: $(OBJS) src/memory.ld guest/hello.img
	$(LD) -r -b binary guest/hello.img -o hello-img.o
	$(LD) $(LDFLAGS) -T src/memory.ld -o $@ $(OBJS) hello-img.o

qemu: mvmm
	$(QEMU) $(QEMUOPTS)

gdb: mvmm
	$(QEMU) -S -gdb tcp::1234 $(QEMUOPTS)

clean:
	make -C guest clean
	$(RM) $(OBJS) mvmm *.img *.o

.PHONY: qemu gdb clean
