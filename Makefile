CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-gcc

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

KERNEL_OBJECT_NAMES = boot idt/idt_asm kmain screen/framebuffer screen/print util/util gdt/gdt idt/idt mem/paging mem/mem mem/heap util/string util/maths
DRIVER_OBJECT_NAMES = keyboard timer
FS_OBJECT_NAMES = initrd fs
OBJECT_NAMES = $(patsubst %,kernel/%,$(KERNEL_OBJECT_NAMES)) $(patsubst %,driver/%,$(DRIVER_OBJECT_NAMES)) $(patsubst %,fs/%,$(FS_OBJECT_NAMES))
OBJECTS = $(patsubst %,$(OBJ_DIR)/%.o,$(OBJECT_NAMES))

C_FLAGS = -std=gnu99 -Isrc/inc -nostdlib -ffreestanding -lgcc -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -c
AS_FLAGS =
LD_FLAGS = -lgcc -ffreestanding -O2 -nostdlib

KERNEL_OUTPUT = $(BUILD_DIR)/iso/boot/kernel.elf
ISO_OUTPUT = $(BUILD_DIR)/os.iso
MODULES_OUTPUT = $(BUILD_DIR)/iso/modules

MKRD_SRC = src/tools/mkrd.c
INITRD_FILES = initrd/test1.txt initrd/test2.txt
INITRD_OUTPUT = $(MODULES_OUTPUT)/initrd.rd

all: $(ISO_OUTPUT)

ifndef VERBOSE
.SILENT:
endif

$(OBJ_DIR)/%.o: src/%.c
	$(info -> Compiling $<)
	$(eval P := $(shell dirname $@))
	mkdir -p $(P)
	$(CC) $(C_FLAGS) $< -o $@

$(OBJ_DIR)/%.o: src/%.s
	$(info -> Assembling $<)
	$(eval P := $(shell dirname $@))
	mkdir -p $(P)
	$(AS) $(AS_FLAGS) $< -o $@

$(KERNEL_OUTPUT): $(OBJECTS)
	$(info -> Linking objects)
	$(LD) -T src/link.ld $(LD_FLAGS) $(OBJECTS) -o $(KERNEL_OUTPUT)
	grub-file --is-x86-multiboot $(KERNEL_OUTPUT)

initrd: $(INITRD_FILES)
	$(info -> Building initrd)
	mkdir -p $(MODULES_OUTPUT)
	./mkrd $(INITRD_OUTPUT) $(INITRD_FILES)

$(ISO_OUTPUT): $(KERNEL_OUTPUT) initrd
	$(info -> Building .iso)
	grub-mkrescue -o $(ISO_OUTPUT) $(BUILD_DIR)/iso
	#mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -A os -input-charset utf8 -quiet -boot-info-table -o $(ISO_OUTPUT) build/iso

clean:
	rm -rf $(OBJ_DIR)/*
	rm $(KERNEL_OUTPUT)
	rm $(ISO_OUTPUT)
	rm $(INITRD_OUTPUT)

run:
	echo "" > qemu.log
	qemu-system-i386 -cdrom build/os.iso -boot d -serial file:qemu.log

mkrd: $(MKRD_SRC)
	gcc -std=gnu99 -Isrc/inc $(MKRD_SRC) -o mkrd

clean_mkrd:
	rm mkrd