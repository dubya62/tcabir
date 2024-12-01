CC := gcc
CFLAGS := -O3
DBGFLAGS := -g

OBJECTS := main.o lexer.o ArrayList.o
SOURCES := main.c lexer.c ArrayList.c

main: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "###################################"
	./$@ test.c
	@echo "###################################"
	@echo

debug: $(SOURCES)
	$(CC) -o $@ -g $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^ -c

clean:
	rm -rf *.o
	rm main
	rm debug
