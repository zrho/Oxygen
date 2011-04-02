# AMD64 makefile

# Paths
export BIN_DIR=build
export SOURCE_DIR=src

# EMU
export EMU=bochs
export EMUFLAGS = -q

# LD (Default)
export LD=ld
export LDFLAGS=-z max-page-size=0x1000

# ASM32
export ASM32 = nasm
export ASM32FLAGS = -f elf32

# ASM64
export ASM64 = nasm
export ASM64FLAGS = -f elf64

# Builds all subprojects
.PHONY: all
all: loader kernel root

# Remove all binaries
.PHONY: clean
clean:
	$(MAKE) -f kernel/LoaderMakefile clean
	$(MAKE) -f kernel/KernelMakefile clean
	$(MAKE) -f root/RootMakefile clean
	rm boot.iso

# Run the kernel
.PHONY: run
run: iso
	$(EMU) $(EMUFLAGS)

# Target for building the loader binary
.PHONY: loader
loader:
	$(MAKE) -f kernel/LoaderMakefile all
	
# Target for building the kernel binary
.PHONY: kernel
kernel:
	$(MAKE) -f kernel/KernelMakefile all
	
# Target for building the root binary
.PHONY: root
root:
	$(MAKE) -f root/RootMakefile all
	
# Create the iso
.PHONY: iso
iso: all
	cp kernel/build/kernel/kernel64.bin \
	   kernel/build/loader/loader64.bin \
	   root/build/root64.bin \
	   kernel/iso/boot/
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 \
	-boot-info-table -o boot.iso kernel/iso
