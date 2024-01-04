CC = mpicc
CFLAGS = -Wall -Werror -Wno-error=unused-variable

SRCDIR = .
SRCS = $(shell find $(SRCDIR) -name "*.c")
BIN = tema3
TXT = swarm*.txt tracker*.txt

ifeq ($(debug),1)
	CFLAGS += -DDEBUG
endif

build: $(BIN)

$(BIN): $(SRCS) 
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf $(BIN) $(TXT)
