QEMU = qemu-system-aarch64
CPUS = 1

TARGET = kernel

QEMUOPTS = -cpu cortex-a72 -machine virt,gic-version=2 -kernel $(TARGET) -m 128M -smp $(CPUS) -nographic
QEMUOPTS += -drive file=fs.img,if=none,format=raw,id=x0
QEMUOPTS += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

qemu: kernel fs.img
	$(QEMU) $(QEMUOPTS)
