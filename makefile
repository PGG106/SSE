
NETWORK_NAME = nn.net
_THIS     := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
_ROOT     := $(_THIS)
EVALFILE   = $(NETWORK_NAME)
CC        := gcc-14
TARGET    := Alexandria
WARNINGS   = -Wall -Wcast-qual -Wextra -Wshadow -Wdouble-promotion -Wformat=2 -Wnull-dereference -Wlogical-op -Wold-style-cast -Wundef -pedantic
CFLAGS  :=  -O3 -flto -fno-exceptions -fno-stack-protector -DNDEBUG $(WARNINGS)
NATIVE     	 = -march=native
AVX2FLAGS    = -DUSE_AVX2 -DUSE_SIMD -mavx2 -mbmi -mfma
BMI2FLAGS    = -DUSE_AVX2 -DUSE_SIMD -mavx2 -mbmi -mbmi2 -mfma
AVX512FLAGS  = -DUSE_AVX512 -DUSE_SIMD -mavx512f -mavx512bw -mfma
VNNI512FLAGS = -DUSE_VNNI512 -DUSE_AVX512 -DUSE_SIMD -mavx512f -mavx512bw -mavx512vnni -mfma


# engine name
NAME      := Alexandria

TMPDIR = .tmp


#KAGGLE build
ifeq ($(KAGGLE), true)
	CFLAGS += -DKAGGLE
endif

ifeq ($(BENCH), true)
	CFLAGS += -DBENCH
endif

ifeq ($(NOPONDER), true)
	CFLAGS += -DNOPONDER
endif

ifeq ($(NOSTDLIB), true)
	CFLAGS += -nostdlib -ffreestanding -fno-tree-loop-distribute-patterns -static -DNOSTDLIB
else
	CFLAGS += -lm
endif

#UCI build
ifeq ($(UCI), true)
	CFLAGS += -DUCI
endif

# Detect Clang
ifeq ($(CC), clang)
#CFLAGS = -funroll-loops -O3 -flto -fuse-ld=lld -fno-exceptions -std=gnu++2a -DNDEBUG // TODO: restore clang
endif

# Detect Windows
ifeq ($(OS), Windows_NT)
	MKDIR    := mkdir
else
ifeq ($(COMP), MINGW)
	MKDIR    := mkdir
else
	MKDIR   := mkdir -p
endif
endif


# Detect Windows
ifeq ($(OS), Windows_NT)
	uname_S := Windows
	SUFFIX  := .exe
	CFLAGS += -static
else
	SUFFIX  :=
	uname_S := $(shell uname -s)
endif


FLAGS_DETECTED =
PROPERTIES = $(shell echo | $(CC) -march=native -E -dM -)

ifneq ($(findstring __AVX2__, $(PROPERTIES)),)
	FLAGS_DETECTED = $(AVX2FLAGS)
endif

ifneq ($(findstring __BMI2__, $(PROPERTIES)),)
	FLAGS_DETECTED = $(BMI2FLAGS)
endif

ifneq ($(findstring __AVX512F__, $(PROPERTIES)),)
	ifneq ($(findstring __AVX512BW__, $(PROPERTIES)),)
		FLAGS_DETECTED = $(AVX512FLAGS)
	endif
endif

ifneq ($(findstring __AVX512VNNI__, $(PROPERTIES)),)
	FLAGS_DETECTED = $(VNNI512FLAGS)
endif

# Remove native for builds
ifdef build
	NATIVE =
else
	CFLAGS += $(FLAGS_DETECTED)
endif

# SPECIFIC BUILDS
ifeq ($(build), native)
	NATIVE   = -march=native
	ARCH     = -x86-64-native
endif

# SPECIFIC BUILDS
ifeq ($(build), native)
	NATIVE     = -march=native
	ARCH       = -x86-64-native
	CFLAGS += $(FLAGS_DETECTED)
endif

ifeq ($(build), x86-64)
	NATIVE       = -mtune=znver1
	INSTRUCTIONS = -msse -msse2 -mpopcnt
	ARCH         = -x86-64
endif

ifeq ($(build), x86-64-modern)
	NATIVE       = -mtune=znver2
	INSTRUCTIONS = -m64 -msse -msse3 -mpopcnt
	ARCH         = -x86-64-modern
endif

ifeq ($(build), x86-64-avx2)
	NATIVE    = -march=bdver4 -mno-tbm -mno-sse4a -mno-bmi2
	ARCH      = -x86-64-avx2
	CFLAGS += $(AVX2FLAGS)
endif

ifeq ($(build), x86-64-bmi2)
	NATIVE    = -march=haswell
	ARCH      = -x86-64-bmi2
	CFLAGS += $(BMI2FLAGS)
endif

ifeq ($(build), x86-64-avx512)
	NATIVE    = -march=x86-64-v4 -mtune=znver4
	ARCH      = -x86-64-avx512
	CFLAGS += $(AVX512FLAGS)
endif

ifeq ($(build), x86-64-vnni512)
	NATIVE    = -march=x86-64-v4 -mtune=znver4
	ARCH      = -x86-64-vnni512
	CFLAGS += $(VNNI512FLAGS)
endif

# Add network name and Evalfile
CFLAGS += -DNETWORK_NAME=\"$(NETWORK_NAME)\" -DEVALFILE=\"$(EVALFILE)\"

SOURCES := $(wildcard src/*.c) $(wildcard src/*.cpp)
OBJECTS := $(patsubst %,$(TMPDIR)/%.o,$(SOURCES))
DEPENDS := $(patsubst %,$(TMPDIR)/%.d,$(SOURCES))
EXE     := $(NAME)$(SUFFIX)

all: $(TARGET)
clean:
	@rm -rf $(TMPDIR) *.o  $(DEPENDS) *.d *.tar *.gz *.xz

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(NATIVE) -MMD -MP -o $(EXE) $^ $(FLAGS) -lm

$(TMPDIR)/%.c.o: %.c | $(TMPDIR)
	$(CC) $(CFLAGS) $(NATIVE) -MMD -MP -c $< -o $@ $(FLAGS)

$(TMPDIR):
	$(MKDIR) "$(TMPDIR)" "$(TMPDIR)/src"

-include $(DEPENDS)

cleanall: clean all

small: cleanall
	sstrip $(EXE)
	ls -la $(EXE)
	xz -k -9 $(EXE)
	xz -k -9 $(NETWORK_NAME)
	tar -cf submission.tar $(EXE).xz $(NETWORK_NAME).xz main.py
	ls -la submission.tar
	zopfli --i1000 submission.tar
	ls -la submission.tar.gz
