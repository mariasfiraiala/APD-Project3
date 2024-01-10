CC = mpicc
CFLAGS = -Wall -Werror -Wno-error=unused-variable -g

SRCDIR = .
SRCS = $(shell find $(SRCDIR) -name "*.c")
BIN = tema3

ifeq ($(debug),1)
	CFLAGS += -DDEBUG
endif

build: $(BIN)

$(BIN): $(SRCS) 
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf $(BIN)
