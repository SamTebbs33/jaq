CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-gcc
MKISO = grub-mkrescue
GRUBFILE = grub-file
EMU = qemu-system-i386
DEBUGGER = gdb

CC_FLAGS ?= -DARCH=\"$(ARCH)\" -std=gnu99 -Isrc/inc -ffreestanding -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -lgcc -O0 $(EXTRA_CC_FLAGS)
AS_FLAGS ?= $(EXTRA_AS_FLAGS)
LD_FLAGS ?= -ffreestanding -O2 -nostdlib -lgcc $(EXTRA_LD_FLAGS)
EMU_FLAGS ?= -cdrom $(ISO_OUTPUT) -boot d -serial stdio $(EXTRA_EMU_FLAGS)
GRUBFILE_FLAGS ?= --is-x86-multiboot $(EXTRA_GRUBFILE_FLAGS)
DEBUGGER_FLAGS ?= -ex "symbol-file $(KERNEL_OUTPUT)" -ex "target remote localhost:1234" $(EXTRA_DEBUGGER_FLAGS)

BUILD_DIR ?= build
OBJ_DIR = $(BUILD_DIR)/obj

KERNEL_OBJECT_NAMES = kmain screen/framebuffer screen/print mem/mem mem/heap util/string util/maths util/linkedlist util/tree log/log
DRIVER_OBJECT_NAMES = keyboard timer serial
FS_OBJECT_NAMES = initrd fs devfs

KERNEL_OUTPUT = $(BUILD_DIR)/iso/boot/kernel.elf
ISO_OUTPUT = $(BUILD_DIR)/os.iso
MODULES_OUTPUT = $(BUILD_DIR)/iso/modules

MKRD_SRC = src/tools/mkrd.c
MKRD_OUTPUT = $(BUILD_DIR)/mkrd

INITRD_FILES = initrd/keymaps/macbook_en_GB.txt
INITRD_OUTPUT = $(MODULES_OUTPUT)/initrd.rd

LINK_SCRIPT = src/link.ld
GRUB_CFG ?= grub-files/grub.cfg
STAGE2 = grub-files/stage2_eltorito
GRUB_FILES = $(GRUB_CFG) $(STAGE2)

CHECK_MULTIBOOT ?= 1

all: $(ISO_OUTPUT)

ARCH ?= $(shell arch)

include src/arch/$(ARCH)/Makefile

OBJECT_NAMES = $(patsubst %,kernel/%,$(KERNEL_OBJECT_NAMES)) $(patsubst %,kernel/driver/%,$(DRIVER_OBJECT_NAMES)) $(patsubst %,kernel/fs/%,$(FS_OBJECT_NAMES)) $(patsubst %,arch/x86/%,$(ARCH_OBJECT_NAMES))
OBJECTS = $(patsubst %,$(OBJ_DIR)/%.o,$(OBJECT_NAMES))

ifneq ($(VERBOSE), 1)
.SILENT:
endif

ifdef EMU_LOG
EMU_FLAGS += -d $(EMU_LOG)
endif

ifeq ($(DEBUG), 1)
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
	mkdir -p $(shell dirname $(KERNEL_OUTPUT))
	$(LD) -T $(LINK_SCRIPT) $(LD_FLAGS) $(OBJECTS) -o $(KERNEL_OUTPUT)

$(INITRD_OUTPUT): $(INITRD_FILES) $(MKRD_OUTPUT)
	$(info -> Building initrd)
	mkdir -p $(MODULES_OUTPUT)
	./$(MKRD_OUTPUT) $(INITRD_OUTPUT) $(INITRD_FILES)

$(ISO_OUTPUT): $(KERNEL_OUTPUT) check-multiboot $(INITRD_OUTPUT) $(GRUB_FILES)
	$(info -> Building .iso)
	mkdir -p $(BUILD_DIR)/iso/boot/grub
	cp $(GRUB_FILES) $(BUILD_DIR)/iso/boot/grub/
	$(MKISO) -o $(ISO_OUTPUT) $(BUILD_DIR)/iso

$(MKRD_OUTPUT): $(MKRD_SRC)
	$(info -> Compiling $<)
	gcc -std=gnu99 -Isrc/inc $(MKRD_SRC) -o $(MKRD_OUTPUT)

check-multiboot:

ifeq ($(CHECK_MULTIBOOT),1)
check-multiboot:
	$(GRUBFILE) $(GRUBFILE_FLAGS) $(KERNEL_OUTPUT)
endif

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
