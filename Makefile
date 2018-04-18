BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

KERNEL_OBJECT_NAMES = boot kmain framebuffer print util gdt idt idt_asm paging mem heap
DRIVER_OBJECT_NAMES = keyboard timer
FS_OBJECT_NAMES = initrd
OBJECT_NAMES = $(patsubst %,kernel/%,$(KERNEL_OBJECT_NAMES)) $(patsubst %,driver/%,$(DRIVER_OBJECT_NAMES)) $(patsubst %,fs/%,$(FS_OBJECT_NAMES))
OBJECTS = $(patsubst %,$(OBJ_DIR)/%.o,$(OBJECT_NAMES))

C_FLAGS = -m32 -Isrc/inc -nostdlib -ffreestanding -lgcc -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -c
AS_FLAGS = -f elf

KERNEL_OUTPUT = $(BUILD_DIR)/iso/boot/kernel.elf
ISO_OUTPUT = $(BUILD_DIR)/os.iso

MKRD_SRC = src/tools/mkrd.c
INITRD_FILES = initrd/test1.txt initrd/test2.txt
INITRD_OUTPUT = $(BUILD_DIR)/iso/modules/initrd.rd

all: $(ISO_OUTPUT)

obj_dirs:
	mkdir -p $(OBJ_DIR)/kernel
	mkdir -p $(OBJ_DIR)/driver
	mkdir -p $(OBJ_DIR)/fs

$(OBJ_DIR)/%.o: src/%.c obj_dirs
	gcc $(C_FLAGS)  $< -o $@

$(OBJ_DIR)/%.o: src/%.s obj_dirs
	nasm $(AS_FLAGS) $< -o $@

kernel.elf: $(OBJECTS)
	ld -T src/link.ld -melf_i386 $(OBJECTS) -o $(KERNEL_OUTPUT)

$(ISO_OUTPUT): kernel.elf
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -A os -input-charset utf8 -quiet -boot-info-table -o $(ISO_OUTPUT) build/iso

clean:
	rm -rf $(OBJ_DIR)/*
	rm $(KERNEL_OUTPUT)
	rm $(ISO_OUTPUT)

run:
	VBoxManage startvm Samix
	#VBoxManage debugvm "Samix" log

mkrd: $(MKRD_SRC) $(INITRD_FILES)
	gcc -Isrc/inc $(MKRD_SRC) -o mkrd
	./mkrd $(INITRD_OUTPUT) $(INITRD_FILES)