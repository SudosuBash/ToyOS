include $(TOP_DIR)/config.mk

TARGET_OBJS := driver.o
SOURCE_OBJS := chr_dev.o

all: $(TARGET_OBJS)

chr_dev.o: chr_dev.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "	CC" $<

$(TARGET_OBJS): $(SOURCE_OBJS)
	@$(LD) $(LDFLAGS_OBJ) $@ $^ 
	@echo "	LD" $<