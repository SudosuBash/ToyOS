AS      := nasm
CC 		:= gcc
LD      := ld
MAKE    := make
PY      := python3
OBJCOPY	:= objcopy

IMAGE      := $(TOP_DIR)/myos.img
ARCH := x86
ARCH_DIR := $(TOP_DIR)/arch/$(ARCH)
ARCH_GENERATED_DIR := $(ARCH_INCLUDE_DIR)/generated

INCLUDE_DIR := $(TOP_DIR)/include
SRC_DIR  := $(TOP_DIR)/src
DRV_DIR	 := $(TOP_DIR)/drivers

CINCLUDE := -I$(INCLUDE_DIR) -I$(ARCH_DIR)/include
CFLAGS := -m64 -O2 -std=gnu11 -ffreestanding -fno-stack-protector -nostdlib -ffunction-sections -fdata-sections \
        $(CINCLUDE) -g  -mcmodel=kernel -fno-pic -fno-omit-frame-pointer -mno-sse -mno-mmx -mno-sse2 -mno-sse3 -mno-3dnow -mno-red-zone -Wall \
		-D__KERNEL__  

ASFLAGS := -Wa,--noexecstack 

CFLAGS_M32 := -m32 -ffreestanding -fno-stack-protector -nostdlib $(CINCLUDE) -g -O0 -Wall -Werror
LDFLAGS_KERNEL  := -z max-page-size=4096 -m elf_x86_64 -T $(ARCH_DIR)/kernel/klinker.lds -e _start --gc-sections -z noexecstack --no-warn-execstack
LDFLAGS_OBJ := -r -z noexecstack --no-warn-execstack