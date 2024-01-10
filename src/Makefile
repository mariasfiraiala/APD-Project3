CC = mpicc
CFLAGS = -Wall -Werror -Wno-error=unused-variable -g

SRCDIR = .
SRCS = $(shell find $(SRCDIR) -name "*.c")
BIN = tema3

build: $(BIN)

$(BIN): $(SRCS) 
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf $(BIN)
