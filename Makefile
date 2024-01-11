CC = mpicc
CFLAGS = -Wall -Werror -g -I include/

SRCDIR = src
SRCS = $(shell find $(SRCDIR) -name "*.c")
BIN = tema3

build: $(BIN)

$(BIN): $(SRCS) 
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf $(BIN)
