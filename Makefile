CC      = gcc
LD      = ld
INCLUDE = include
SRC     = src
BIN     = bin

CFLAGS  = -g -Wall -m32 -fPIC -c
IFLAGS  = -I$(INCLUDE)
LDFLAGS = -melf_i386 -lc -shared

CSRCS   = $(wildcard $(SRC)/*.c)
SSRCS   = $(wildcard $(SRC)/*.s)
COBJS   = $(patsubst %.c,%.o,$(CSRCS))
SOBJS   = $(patsubst %.s,%.o,$(SSRCS))
LIB     = $(BIN)/libfthread.so

.PHONY: all
all: $(LIB)

$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) $(IFLAGS) -o $@ $^

$(SOBJS): %.o : %.s
	$(CC) $(CFLAGS) $(IFLAGS) -o $@ $^

$(LIB): $(COBJS) $(SOBJS)
	$(LD) $(LDFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(COBJS) $(SOBJS) $(LIB)
