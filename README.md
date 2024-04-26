nasm -f bin boot.asm -o boot.bin

qemu-system-x86_64 -hda boot.bin


## To copy your bootloader to a USB flash drive
sudo dd if=./boot.bin of=/dev/WHERE_IT_IS_MOUNTED

## To actually boot into it on your PC
Simply choose the USB drive from your boot options