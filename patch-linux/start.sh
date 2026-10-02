qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a57 \
    -m 2G \
    -kernel arch/arm64/boot/Image \
    -append "console=ttyAMA0 root=/dev/ram init=/init nokaslr" \
    -initrd ../initramfs.cpio \
    -nographic \
    -virtfs local,path=../test,mount_tag=hostshare,security_model=none \
    -s

