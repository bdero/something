GENDEV?=/opt/toolchains/gen
GENGCC_BIN=$(GENDEV)/m68k-elf/bin/m68k-elf-
GENBIN=$(GENDEV)/bin/

PROJECT_NAME = something

CC = $(GENGCC_BIN)gcc
AS = $(GENGCC_BIN)as
LD = $(GENGCC_BIN)ld
NM = $(GENGCC_BIN)nm
OBJC = $(GENGCC_BIN)objcopy
RESCOMP= $(GENBIN)rescomp

INCS = -I$(GENDEV)/m68k-elf/include -I$(GENDEV)/m68k-elf/m68k-elf/include -Isrc -Ires -Iinc
CCFLAGS = $(OPTION) -m68000 -Wall -Wextra -std=c99 -c -fno-builtin
ASFLAGS = -m68000 --register-prefix-optional
LIBS = -L$(GENDEV)/m68k-elf/lib -L$(GENDEV)/m68k-elf/m68k-elf/lib -lmd -lnosys
LINKFLAGS = -T $(GENDEV)/ldscripts/sgdk.ld -nostdlib
ARCHIVES = $(GENDEV)/m68k-elf/lib/gcc/m68k-elf/*/libgcc.a

BOOTSS=$(wildcard src/boot/*.s)
BOOT_RESOURCES=$(BOOTSS:.s=.o)

RESS=$(wildcard res/*.res)
CS=$(wildcard src/*.c)
SS=$(wildcard src/*.s)
RESOURCES=$(RESS:.res=.o)
RESOURCES+=$(CS:.c=.o)
RESOURCES+=$(SS:.s=.o)

OBJS = $(RESOURCES)

.PHONY: all release clean
.SECONDARY: $(PROJECT_NAME).elf

all: release

release: CCFLAGS += -O3 -fomit-frame-pointer
release: LIBMD = $(GENDEV)/m68k-elf/lib/libmd.a
release: main-build
main-build: $(PROJECT_NAME).bin symbol.txt

symbol.txt: $(PROJECT_NAME).bin
	$(NM) -n $(PROJECT_NAME).elf > symbol.txt

src/boot/sega.o: src/boot/rom_head.bin
	$(AS) $(ASFLAGS) src/boot/sega.s -o $@

%.bin: %.elf
	$(OBJC) -O binary $< temp.bin
	dd if=temp.bin of=$@ bs=8K conv=sync

%.elf: $(OBJS) $(BOOT_RESOURCES)
	$(CC) -o $@ $(LINKFLAGS) $(BOOT_RESOURCES) $(LIBMD) $(ARCHIVES) $(OBJS) $(LIBS)

%.o: %.c
	@echo "CC $<"
	@$(CC) $(CCFLAGS) $(INCS) -c $< -o $@

%.o: %.s
	@echo "AS $<"
	@$(AS) $(ASFLAGS) $< -o $@

%.s: %.res
	$(RESCOMP) $< $@

src/boot/rom_head.bin: src/boot/rom_head.o
	$(LD) $(LINKFLAGS) --oformat binary -o $@ $<

clean:
	rm -f $(RESOURCES)
	rm -f $(PROJECT_NAME).bin $(PROJECT_NAME).elf temp.bin symbol.txt
	rm -f src/boot/sega.o src/boot/rom_head.o src/boot/rom_head.bin
	rm -f res/resources.h res/resources.s
	rm -f saves.zip saves.tar.gz
