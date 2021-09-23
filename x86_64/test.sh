qemu-system-x86_64 \
-kernel bzImage \
-hda rootfs.ext2 \
-append "root=/dev/sda rw console=ttyS0" \
--enable-kvm \
--nographic
