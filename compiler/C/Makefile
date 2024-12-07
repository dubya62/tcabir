
BINARY :=bin

TESTFILE :=tests/test.c

CC :=gcc

OPT :=-O3


INCDIRS :=.
CODEDIRS :=. ./lib
OBJDIRS :=objects

INCFLAGS :=$(foreach D, $(INCDIRS), -I$(D))
CFLAGS :=$(OPT) $(INCFLAGS)
DBGFLAGS :=$(INCFLAGS) -g

SOURCES :=$(foreach D, $(CODEDIRS), $(wildcard $(D)/*.c))
OBJECTS :=$(patsubst ./%.c, objects/%.o, $(SOURCES))

.PHONY: all FORCE

all: test

test: $(BINARY) $(TESTFILE)
	./$(BINARY) $(TESTFILE) $(INCFLAGS)

debug: CFLAGS :=$(DBGFLAGS)
debug: clean $(BINARY)
	gdb $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(CFLAGS)

objects/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm $(BINARY)
	-rm $(OBJECTS)


