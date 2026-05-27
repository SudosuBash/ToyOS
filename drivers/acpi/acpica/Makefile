include $(TOP_DIR)/config.mk


TARGET = libacpi.o

all: $(TARGET)

# 链接成静态库
$(TARGET): $(OBJS)
	@echo "	LD $@"
	@$(LD) $(LDFLAGS) -r -o $@ $^

# 编译每个 .c 文件
$(TOOLS_SRC_DIR)/%.o: $(TOOLS_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "	CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	@rm -rf $(OBJS) $(TARGET)

.PHONY: all clean