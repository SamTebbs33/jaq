BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

OBJECT_NAMES = boot kmain framebuffer print util
OBJECTS = $(patsubst %,$(OBJ_DIR)/%.o,$(OBJECT_NAMES))

C_FLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
             -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -Wno-unused-parameter -c
AS_FLAGS = -f elf

KERNEL_OUTPUT = $(BUILD_DIR)/iso/boot/kernel.elf
ISO_OUTPUT = $(BUILD_DIR)/os.iso

all: $(ISO_OUTPUT)

$(OBJ_DIR)/%.o: src/kernel/%.c
	gcc $(C_FLAGS)  $< -o $@

$(OBJ_DIR)/%.o: src/kernel/%.s
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