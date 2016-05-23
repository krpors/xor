#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cencode.h"
#include "cdecode.h"

#define ENCODE_SIZE 1024

// encode encodes the input string to base64 and returns it as a
// null terminated string of characters.
static char* encode(const char* input) {
	// Allocate output buffer
	char* output = malloc(ENCODE_SIZE * sizeof(char));
	// c is used to up a pointer location. 
	char* c = output;

	int cnt = 0;
	base64_encodestate state;

	// Beginning of encoding.
	base64_init_encodestate(&state);
	// Somehow, using 'c' fills 'output'?
	cnt = base64_encode_block(input, strlen(input), c, &state);
	c += cnt;
	cnt = base64_encode_blockend(c, &state);
	c += cnt;

	*c = 0;

	return output;
}

// decode decodes the input string.
static char* decode(const char* input) {
	char* output = malloc(ENCODE_SIZE * sizeof(char));
	return output;
}

int main(int argc, char* argv[]) {
	char* str = "kevin pors";
	char* encoded = encode(str);

	printf("%s encoded is %s\n", str, encoded);

	return 0;
}
