qemu-system-aarch64 \
-machine virt \
-cpu cortex-a57 \
-kernel ./Image \
-initrd rootfs.cpio \
--nographic 

