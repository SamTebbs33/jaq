CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-gcc
MKISO = grub-mkrescue
GRUBFILE = grub-file
EMU = qemu-system-i386
DEBUGGER = gdb

CC_FLAGS = -std=gnu99 -Isrc/inc -ffreestanding -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -lgcc -O0
AS_FLAGS =
LD_FLAGS = -ffreestanding -O2 -nostdlib -lgcc
EMU_FLAGS = -cdrom $(ISO_OUTPUT) -boot d -serial stdio
GRUBFILE_FLAGS = --is-x86-multiboot
DEBUGGER_FLAGS = -ex "symbol-file $(KERNEL_OUTPUT)" -ex "target remote localhost:1234"

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

KERNEL_OBJECT_NAMES = boot idt/idt_asm kmain screen/framebuffer screen/print util/util gdt/gdt idt/idt mem/mem mem/heap util/string util/maths log/log mem/paging idt/exceptions
DRIVER_OBJECT_NAMES = keyboard timer serial
FS_OBJECT_NAMES = initrd fs
OBJECT_NAMES = $(patsubst %,kernel/%,$(KERNEL_OBJECT_NAMES)) $(patsubst %,driver/%,$(DRIVER_OBJECT_NAMES)) $(patsubst %,fs/%,$(FS_OBJECT_NAMES))
OBJECTS = $(patsubst %,$(OBJ_DIR)/%.o,$(OBJECT_NAMES))

KERNEL_OUTPUT = $(BUILD_DIR)/iso/boot/kernel.elf
ISO_OUTPUT = $(BUILD_DIR)/os.iso
MODULES_OUTPUT = $(BUILD_DIR)/iso/modules

MKRD_SRC = src/tools/mkrd.c
MKRD_OUTPUT = $(BUILD_DIR)/mkrd

INITRD_FILES = initrd/test1.txt initrd/test2.txt
INITRD_OUTPUT = $(MODULES_OUTPUT)/initrd.rd

LINK_SCRIPT = src/link.ld
GRUB_CFG = $(BUILD_DIR)/iso/boot/grub/grub.cfg

all: $(ISO_OUTPUT)

ifndef VERBOSE
.SILENT:
endif

ifdef EMU_LOG
EMU_FLAGS += -d int
endif

ifdef DEBUG
CC_FLAGS += -g
AS_FLAGS += --gstabs+
EMU_FLAGS += -s -S
endif

$(OBJ_DIR)/%.o: src/%.c
	$(info -> Compiling $<)
	$(eval P := $(shell dirname $@))
	mkdir -p $(P)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: src/%.s
	$(info -> Assembling $<)
	$(eval P := $(shell dirname $@))
	mkdir -p $(P)
	$(AS) $(AS_FLAGS) $< -o $@

$(KERNEL_OUTPUT): $(OBJECTS) $(LINK_SCRIPT)
	$(info -> Linking objects)
	$(LD) -T $(LINK_SCRIPT) $(LD_FLAGS) $(OBJECTS) -o $(KERNEL_OUTPUT)
	$(GRUBFILE) $(GRUBFILE_FLAGS) $(KERNEL_OUTPUT)

$(INITRD_OUTPUT): $(INITRD_FILES) $(MKRD_OUTPUT)
	$(info -> Building initrd)
	mkdir -p $(MODULES_OUTPUT)
	./$(MKRD_OUTPUT) $(INITRD_OUTPUT) $(INITRD_FILES)

$(ISO_OUTPUT): $(KERNEL_OUTPUT) $(INITRD_OUTPUT) $(GRUB_CFG)
	$(info -> Building .iso)
	$(MKISO) -o $(ISO_OUTPUT) $(BUILD_DIR)/iso

$(MKRD_OUTPUT): $(MKRD_SRC)
	$(info -> Compiling $<)
	gcc -std=gnu99 -Isrc/inc $(MKRD_SRC) -o $(MKRD_OUTPUT)

clean:
	rm -rf $(OBJ_DIR)/*
	rm $(KERNEL_OUTPUT)
	rm $(ISO_OUTPUT)
	rm $(INITRD_OUTPUT)

debug:
	$(DEBUGGER) $(DEBUGGER_FLAGS)

run:
	$(info -> Running qemu)
	$(EMU) $(EMU_FLAGS)

clean_mkrd:
	rm mkrd
