# Jaq
A hobby operating system being developed for my own interest and learning.

Credit goes to the OSDev [forums](https://forums.osdev.org) and [wiki](https://wiki.osdev.org) for their help and useful articles.

# Building and running
Building the project requires a working i686-elf gcc toolchain, along with the standard grub tools. Change the paths at the top of the Makefile to point to your installations.
Running the project requires QEMU. Change the executable path at the top of the Makefile if needed.

From the project root:
```$bash
make
make run
```

To delete the Makefile's output:
```
make clean
```

