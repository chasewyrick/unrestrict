TARGET  = Unrestrict.dylib
OUTDIR ?= bin
DSYMDIR ?= dsym
PREFIX ?= /Library/MobileSubstrate/ServerPlugins
ARCHS  ?= arm64 arm64e
SRC	= src/main.c src/KernelMemory.c src/KernelUtilities.c src/KernelExecution.c src/KernelOffsets.c src/utils.c src/kernel_call/kc_parameters.c src/kernel_call/kernel_alloc.c src/kernel_call/kernel_call.c src/kernel_call/kernel_memory.c src/kernel_call/kernel_slide.c src/kernel_call/log.c src/kernel_call/pac.c src/kernel_call/parameters.c src/kernel_call/platform.c src/kernel_call/platform_match.c src/kernel_call/user_client.c external/offset-cache/offsetcache.c
OBJ	= $(SRC:.c=.o)

CC      = xcrun -sdk iphoneos gcc $(patsubst %,-arch %,$(ARCHS))
LDID    = ldid
CFLAGS  = -Iinclude -Isrc -Isrc/kernel_call -Iexternal/offset-cache -Wno-deprecated-declarations -DHAVE_MAIN -g
LDFLAGS = -framework IOKit -framework CoreFoundation

ifeq ($(DEBUG),1)
CFLAGS += -DDEBUG
endif

.PHONY: all install clean

all: $(OUTDIR)/$(TARGET)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $^

install: all
	install -d "$(DESTDIR)$(PREFIX)"
	install $(OUTDIR)/$(TARGET) "$(DESTDIR)$(PREFIX)"

$(OUTDIR):
	mkdir -p $@

$(DSYMDIR):
	mkdir -p $@

$(OUTDIR)/$(TARGET): $(OBJ) | $(OUTDIR) $(DSYMDIR)
	$(CC) $(LDFLAGS) -dynamiclib -install_name $(PREFIX)/$(TARGET) -o $@ $^
	dsymutil $@ -out $(DSYMDIR)/$(TARGET).dSYM
	strip -S $@
	$(LDID) -S $@

install: all

clean:
	rm -rf $(OUTDIR) $(OBJ)
