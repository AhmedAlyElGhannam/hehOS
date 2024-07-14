# variables in makefiles are basically copy pasta
FILES = ./build/kernel.asm.o


# $() is how you pasta variable contents like bash
# the 4th line pads os.bin with 100 sectors of 512 bytes each equals  zeros 
all: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=512 count=100 >> ./bin/os.bin 

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

# in makefile, the stuff before : are the label
# the stuff after the : are the dependency(ies) required by said label
# to build correctly
./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o


launch: 
	qemu-system-x86_64 -hda ./bin/os.bin

clean:
	rm -rf ./bin/boot.bin 
	rm -rf ./bin/kernel.bin
	rm -rf ./bin/os.bin
	rm -rf $(FILES)
	rm -rf ./build/kernelfull.o