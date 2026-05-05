CC = gcc-7
AR = ar

AR_OBJS = acpica/libacpi.a
OUTPUT_OBJS = libtools.a

all: $(OUTPUT_OBJS)

$(OUTPUT_OBJS): $(AR_OBJS)
	@echo "	AR $@"
	@$(AR) rcs $@ $^

$(AR_OBJS): 
	@$(MAKE) -C src/acpica

clean:
	@rm -rf ./libtools.a
	@$(MAKE) -C src/acpica clean

.PHONY: all clean