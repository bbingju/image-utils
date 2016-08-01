BIN = pbmtoascii
SRCS = pbmtoascii.c
CFLAGS = -Wall -O2

all: $(BIN)

$(BIN): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(BIN) *.o *~

