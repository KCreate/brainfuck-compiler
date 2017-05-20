CC=clang
OPT=-O0
CFLAGS=-g -fdata-sections -ffunction-sections $(OPT)
OBJS=build/bfc.o

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -dead_strip -o bin/bfc

build/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf bin/*
	rm -rf build/*
