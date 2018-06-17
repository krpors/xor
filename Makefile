CFLAGS=-std=c99 -Wall -Wextra -pedantic -O3 -MMD -MP

objects := xor.o cencode.o cdecode.o

all: xor

xor: $(objects)

clean:
	$(RM) $(objects) $(objects:.o=.d) xor

-include $(objects:.o=.d)

.PHONY: all clean