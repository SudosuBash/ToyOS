# 交叉编译工具链定义（对接你的 ToyOS 工具链）
CC = gcc-7
AR = ar

# 目录定义
SRC_DIR = .
ROOT_DIR := $(abspath $(lastword $(MAKEFILE_LIST))/../../../../..)
ARCH = x86
ARCH_DIR = $(ROOT_DIR)/arch/$(ARCH)

CINCLUDE := -I$(ARCH_DIR)/include -I$(ARCH_DIR)/include/devicetree/acpica -I$(ROOT_DIR)/include
CFLAGS = -m64 -O2 -std=gnu11 -ffreestanding -fno-stack-protector -nostdlib \
        $(CINCLUDE) -g  -mcmodel=kernel -fno-pic -fno-omit-frame-pointer -mno-sse -mno-mmx -mno-sse2 -mno-sse3 -mno-3dnow -mno-red-zone -Wall \
		-D__KERNEL__ -D__x86_64__ -DACPI_USE_BUILTIN_STDARG -DBITS_PER_LONG=64

# 核心组件列表 (根据你之前的需求挑选)
# 建议先编译：tables, utilities, namespace, hardware, events, parser, executer
COMPONENTS = tables \
             utilities \
             namespace \
             hardware \
             events \
             parser \
             executer \
             resources

# 自动收集所有的 .c 源文件
SRCS = $(foreach dir, $(COMPONENTS), $(wildcard $(SRC_DIR)/$(dir)/*.c))
# 将 .c 文件路径映射为 .o 文件路径
OBJS = $(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, $(SRCS))

# 输出文件名
TARGET = libacpi.a

all: $(TARGET)

# 链接成静态库
$(TARGET): $(OBJS)
	@echo "	AR $@"
	@$(AR) rcs $@ $(OBJS)

# 编译每个 .c 文件
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "	CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	@rm -rf $(OBJS) $(TARGET)

.PHONY: all clean