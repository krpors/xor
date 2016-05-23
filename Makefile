CC=x86_64-w64-mingw32-gcc.exe
CFLAGS=-std=c99 -Wall

DEPS=cencode.h cdecode.h
OBJECTS=main.o cencode.o cdecode.o

# $@ = compilation left side of the :
# $^ = compilation right side of the :
# $< = first item in dependency list

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

xor: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f *.o
