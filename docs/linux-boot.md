## boot

## command
./qemu/build/qemu-system-aarch64 \
                          -M virt \
                          -cpu cortex-a72 \
                          -kernel linux-5.4.83/arch/arm64/boot/Image \
                          -nographic \
                          -append "console=ttyAMA0" \
                          -initrd busybox/rootfs.img \
                          -device virtio-rng-pci,bus=pcie.0,disable-legacy=on,disable-modern=off -m 256

### dtb base addr(x0)
0x48400000

### entrypoint(x4)
0x40080000
