ARCH ?= 64
CFLAGS := -march=haswell -DNDEBUG -flto -O3
LDFILE := 64bit.ld
EXE = sse
NETWORK_NAME = nn.net

ifeq ($(KAGGLE), true)
	CFLAGS += -DKAGGLE
endif

ifeq ($(BENCH), true)
	CFLAGS += -DBENCH
	CFLAGS += -DNOPONDER
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

clean:
	rm -rf build
	rm -f *.map
	rm -f *.o
	rm -f *.c.temp*
	rm -f *.xz
	rm -f *.tar
	rm -f *.gz

all: clean
	gcc $(CFLAGS) -c src/all.c
	ld -T $(LDFILE) -Map=./sse.map -o $(EXE) all.o
	rm *.o
	xz -k -9 $(EXE)
	xz -k -9 $(NETWORK_NAME)
	tar -cf submission.tar $(EXE).xz $(NETWORK_NAME).xz main.py
	ls -la submission.tar
	zopfli --i1000 submission.tar
	ls -la submission.tar.gz
