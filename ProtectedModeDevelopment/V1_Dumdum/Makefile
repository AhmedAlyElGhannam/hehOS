all:
	nasm -f bin ./build/src/boot.asm -o ./bin/boot.bin
	qemu-system-x86_64 -hda ./bin/boot.bin