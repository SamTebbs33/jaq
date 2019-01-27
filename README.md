# Jaq
A kernel (and eventually OS) for the x86 platform.

## Plans
Check out the [GitHub project page](https://github.com/SamTebbs33/jaq/projects/1) for a more detailed and possibly more up-to-date list of plans.

* Support for x86, x86_64 and aarch64 (probably in that order).
* Multithreading task management.
* Microkernel with necessary drivers in initial ramdisk and rest loaded from filesystem.
* Built-in kernel shell with userspace shell as the preferred interface.

More plans can be made (graphics etc.) once viable.

## Building

### Dependencies
* make.
* An i686-elf GNU toolchain (as, gcc, ld etc.). See [this OSDev article](https://wiki.osdev.org/GCC_Cross-Compiler) for instructions.
* The grub tools (grub-mkrescue, grub-file etc.).

### Instructions
This project uses a standard make workflow to build its source code:

In the project root, run `make` to compile the changed source files, optionally adding any of the below flags:
* `DEBUG=1`: Compile with debug symbols on.
* `VERBOSE=1`: Display the commands executed in the Makefile.
* `ARCH=target-arch`: Set the target architecture. Default is the output of the `arch` command. Can be one of:
  * `x86`: Build for x86.
* `GRUB_CFG=/path/to/grub.cfg`: Override the path to the grub.cfg file to include in the build image.
* `BUILD_DIR=/path/to/build/dir`: Override the destination for all built objects. Default is "build".
* `CHECK_MULTIBOOT=0`: Skip the multiboot compatibility check.
* `CC=/path/to/cc`: Override the C compiler used. Default is "i686-elf-gcc".
* `AS=/path/to/as`: Override the assembler used. Default is "i686-elf-as".
* `LD=/path/to/ld`: Override the linker used. Default is "i686-elf-gcc".
* `MKISO=/path/to/mkiso`: Override the program used to create the iso image. Default is "grub-mkrescue".
* `GRUBFILE=/path/to/grub-file`: Override the grub-file program used to check multiboot compatibility. Default is "grub-file".

The flags passed to the programs above can be overridden by setting the `<program>_FLAGS` flag (doesn't apply to `MKISO`), and can be added to by setting `EXTRA_<program>_FLAGS` (generally the preferred option).

## Running

### Dependencies
* qemu-system emulator for your target architecture, such as qemu-system-i386.
* Optionally gdb.

### Instructions
This projects uses a standard make workflow to run the built objects.

In the project root, run `make run` to launch the emulator with the built kernel, optionally adding any of the below flags:
* `DEBUG=1`: Wait for a debugger connection before executing. Follow up with `make debug` in a separate shell.
* `VERBOSE=1`: Display the commands executed in the Makefile.
* `ARCH=target-arch`: Set the target architecture. Default is the output of the `arch` command. Can be one of:
  * `x86`: Build for x86.
* `BUILD_DIR=/path/to/build/dir`: Override the directory in which to look for the built objects. Default is "build".
* `EMU=/path/to/emulator`: Override the emulator used. Default is "qemu-system-i386".
* `DEBUGGER=/path/to/debugger`: Override the debugger used. Default is "gdb".

The flags passed to the programs above can be overridden by setting the `<program>_FLAGS` flag, and can be added to by setting `EXTRA_<program>_FLAGS` (generally the preferred option).

## Structure
All source code files are under the "src" directory. Each architecture has all of its own files in its "arch/x" directory, e.g. all x86-specific code and files are under "arch/x86". All build files are put in "build/".

## Credits
Some credit goes to the OSDev [forums](https://forums.osdev.org) and [wiki](https://wiki.osdev.org) for their help and useful articles, along with all the projects contributors.
