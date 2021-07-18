all:
	nasm boot.asm -f bin -o boot.bin
	gcc -O2 -masm=intel -elf_i386 -ffreestanding -m16 -c -o hi.o hi.c
	ld -m elf_i386 --oformat binary -o hi.bin hi.o
	dd if=/dev/zero of=image.iso bs=512 count=2880
	dd if=./boot.bin of=image.iso conv=notrunc bs=512 seek=0 count=1
	dd if=./hi.bin of=image.iso conv=notrunc bs=512 seek=1 count=2048
	qemu-system-i386 -drive format=raw,file=image.iso
