CC          = gcc
CFLAGS      = -O2 -c -Wall -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26
LDFLAGS     = -lfuse
OBJDIR      = build
SOURCES     = xattrfs.c callbacks.c
OBJECTS     = $(addprefix $(OBJDIR)/, $(SOURCES:%.c=%.o))
EXECUTABLE  = xattrfs

.PHONY: all clean rebuild

all: $(SOURCES) $(EXECUTABLE)

clean:
	@rm -rf $(OBJDIR)

rebuild: clean all

$(EXECUTABLE): $(OBJECTS)
	@echo "LD $@" 
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(OBJDIR)/%.o: %.c
	@echo "CC $<"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@
