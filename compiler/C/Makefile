CC := gcc
CFLAGS := -O3
DBGFLAGS := -g

OBJECTS := main.o lexer.o ArrayList.o token.o normalizer.o HashMap.o
SOURCES := main.c lexer.c ArrayList.c token.c normalizer.c HashMap.c

test: main
	@echo "###################################"
	./$< test.c
	@echo "###################################"
	@echo

main: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

debug: $(SOURCES)
	$(CC) -o $@ -g $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^ -c

clean:
	rm -rf *.o
	rm main
	rm debug
