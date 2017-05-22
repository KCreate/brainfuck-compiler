CC=clang
OPT=-O0
CFLAGS=-g -fdata-sections -ffunction-sections $(OPT)
OBJS=build/bfc.o build/bfast.o build/parser.o

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -dead_strip -o bin/bfc

assemble: bin/bfc
	bin/bfc input.bf > brainfuck.asm
	stackvm build brainfuck.asm -s -o brainfuck.bc
	stackvm run brainfuck.bc

build/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf bin/*
	rm -rf build/*
