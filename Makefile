# AMD64 makefile

# Paths
export BUILD_DIR=build
export BIN_DIR=bin
export SOURCE_DIR=src

# EMU
export EMU=qemu
export EMUFLAGS = -monitor stdio -m 32M -smp 4

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
all: loader kernel

# Remove all binaries
.PHONY: clean
clean:
	rm -r $(BUILD_DIR)/*
	rm boot.iso

# Run the kernel
.PHONY: run
run: iso
	$(EMU) $(EMUFLAGS) -cdrom boot.iso

# Target for building the loader binary
.PHONY: loader
loader:
	$(MAKE) -f LoaderMakefile all
	
# Target for building the kernel binary
.PHONY: kernel
kernel:
	$(MAKE) -f KernelMakefile all
	
# Create the iso
.PHONY: iso
iso: all
	cp $(BUILD_DIR)/kernel/kernel64.bin $(BUILD_DIR)/loader/loader64.bin iso/boot/
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 \
	-boot-info-table -o boot.iso iso
