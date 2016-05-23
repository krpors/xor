#include <stdio.h> // printf ...
#include <stdlib.h> // malloc ...
#include <string.h> // strlen() ...
#include <stdint.h> // uint32_t ...

#include "cencode.h"
#include "cdecode.h"

static const uint16_t ENCODE_SIZE = 1024; // arbitrary buffer size to encode/decode.
static const uint32_t MAGIC       = 0x5f; // magic constant for xoring characters.

static char* encode(const char*);
static char* decode(const char*);
static char* xor(const char*);

// Encodes the input string to base64 and returns it as a null terminated string
// of characters. The returned string is created on the heap, so needs to be
// free()'d manually.
static char* encode(const char* input) {
	// Allocate output buffer
	char* output = malloc(ENCODE_SIZE * sizeof(char));
	// c points to the start of the 'output' string.
	char* c = output;

	base64_encodestate state;

	// Beginning of encoding.
	base64_init_encodestate(&state);
	// Somehow, using 'c' fills 'output'?
	c += base64_encode_block(input, strlen(input), c, &state);
	c += base64_encode_blockend(c, &state);

	*c = 0;

	return output;
}

// Decodes the input string to base64 and returns it as a null terminated string
// of characters. The returned string is created on the heap, so needs to be
// free()'d manually.
static char* decode(const char* input) {
	char* output = malloc(ENCODE_SIZE * sizeof(char));
	char* ptr_output = output;

	base64_decodestate state;

	// Beginning of encoding.
	base64_init_decodestate(&state);
	int cnt = base64_decode_block(input, strlen(input), output, &state);
	ptr_output += cnt;
	*ptr_output = '\0';

	return output;
}

static char* xor(const char* input) {
	char* cruft = malloc(ENCODE_SIZE * sizeof(char));
	for (int i = 0; i < strlen(input); i++) {
		cruft[i] = input[i] ^ MAGIC;
	}
	cruft[strlen(input)] = '\0'; // terminating null byte.

	return cruft;
}

int main(int argc, char* argv[]) {
	char* str = "hello world";
	char* xored = xor(str);
	char* encoded = encode(xored);

	char* a = decode(encoded);
	char* b = xor(a);

	printf("%s xored is '%s'\n", str, xored);
	printf("%s encoded is '%s'\n", str, encoded);
	printf("%s decoded is '%s'\n", encoded, a);
	printf("%s xored is '%s'\n", a, b);

	free(encoded);
	free(xored);
	free(a);
	free(b);

	return 0;
}
