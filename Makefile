CC      = gcc
LD      = ld
INCLUDE = include
SRC     = src
BIN     = bin
EXAMPLE = example

CFLAGS  = -g -Wall -m32 -fPIC -c
IFLAGS  = -I$(INCLUDE)
LDFLAGS = -melf_i386 -lc -ldl -shared

CSRCS   = $(wildcard $(SRC)/*.c)
SSRCS   = $(wildcard $(SRC)/*.s)
COBJS   = $(patsubst %.c,%.o,$(CSRCS))
SOBJS   = $(patsubst %.s,%.o,$(SSRCS))
LIB     = $(BIN)/libfthread.so

ESRCS   = $(wildcard $(EXAMPLE)/*.c)
EBINS   = $(patsubst %.c,%,$(ESRCS))
ECFLAGS = -g -Wall -m32
EIFLAGS = -I$(INCLUDE)
ELFLAGS = -L$(BIN) -Wl,--rpath=$(BIN)

.PHONY: all
all: $(LIB) $(EBINS)

.PHONY: example
example: $(EBINS)

$(COBJS): %.o : %.c
	$(CC) $(CFLAGS) $(IFLAGS) -o $@ $^

$(SOBJS): %.o : %.s
	$(CC) $(CFLAGS) $(IFLAGS) -o $@ $^

$(LIB): $(COBJS) $(SOBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(EBINS): % : %.c $(LIB)
	$(CC) $(ECFLAGS) $(EIFLAGS) $(ELFLAGS) -o $@ $^ -lfthread

.PHONY: clean
clean:
	rm -f $(COBJS) $(SOBJS) $(LIB) $(EBINS)
