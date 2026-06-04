include $(TOP_DIR)/config.mk

TARGET_OBJS := driver.o
SOURCE_OBJS := chr_dev.o

all: $(TARGET_OBJS)

chr_dev.o: chr_dev.c
	@echo "	CC" $<
	@$(CC) $(CFLAGS) -c $< -o $@
	

$(TARGET_OBJS): $(SOURCE_OBJS)
	@echo "	LD" $<
	@$(LD) $(LDFLAGS_OBJ) -o $@ $<