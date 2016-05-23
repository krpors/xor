#include <stdio.h> // printf ...
#include <stdlib.h> // malloc ...
#include <string.h> // strlen() ...
#include <stdint.h> // uint32_t ...
#include <unistd.h> // getopt
#include <stdbool.h>

#include "cencode.h"
#include "cdecode.h"

static const uint16_t ENCODE_SIZE = 1024; // arbitrary buffer size to encode/decode.
static const uint32_t MAGIC       = 0x5f; // magic constant for xoring characters.

static char* encode(const char*);
static char* decode(const char*);
static char* xor(const char*);

// Encode will convert every character in the input string with the magic number,
// then encode it to base64. The returned character string is created on the heap,
// and needs to be freed manually.
static char* encode(const char* input) {
	char* xored = xor(input);

	// Allocate output buffer
	char* output = malloc(ENCODE_SIZE * sizeof(char));
	// c points to the start of the 'output' string.
	char* c = output;

	base64_encodestate state;

	// Beginning of encoding.
	base64_init_encodestate(&state);
	// Somehow, using 'c' fills 'output'?
	c += base64_encode_block(xored, strlen(xored), c, &state);
	c += base64_encode_blockend(c, &state);

	*c = 0;

	free(xored);

	return output;
}

// Decode will decode the input string from base64, then convert every character
// with the magic number. The returned string is created on the heap, and needs
// to be freed manually.
static char* decode(const char* input) {
	char* output = malloc(ENCODE_SIZE * sizeof(char));
	char* ptr_output = output;

	base64_decodestate state;

	// Beginning of encoding.
	base64_init_decodestate(&state);
	int cnt = base64_decode_block(input, strlen(input), output, &state);
	ptr_output += cnt;
	*ptr_output = '\0';

	char* xored = xor(output);

	free(output);

	return xored;
}

// Will xor every character in the input string with the magic number.
static char* xor(const char* input) {
	char* cruft = malloc(ENCODE_SIZE * sizeof(char));
	for (int i = 0; i < strlen(input); i++) {
		cruft[i] = input[i] ^ MAGIC;
	}
	cruft[strlen(input)] = '\0'; // terminating null byte.

	return cruft;
}

// Prints help to the standard out.
static void printhelp() {
	printf("Usage: xor [-e,-d|-h]\n");
	printf("Encodes or decodes Websphere obfuscated strings.\n");
	printf("\n");
	printf("  -h          prints this help and exits\n");
	printf("  -e STRING   encodes the given string\n");
	printf("  -d STRING   decodes the given string\n");
	printf("\n");
	printf("Examples:\n");
	printf("\n");
	printf("$ xor -e \"hello world\"\n");
	printf("hello world: NzozMzB/KDAtMzs=\n");
	printf("$ xor -d \"NzozMzB/KDAtMzs=\"\n");
	printf("NzozMzB/KDAtMzs=: hello world\n");
	printf("\n");
	printf("Full source and docs at <http://github.com/krpors/xor>\n");
}

int main(int argc, char* argv[]) {
	int opt = -1;
	char* enc = NULL;
	char* dec = NULL;
	bool help = false;
	int errcount = 0;

	opterr = 0;
	while ((opt = getopt(argc, argv, ":e:d:h")) != -1) {
		switch (opt) {
		case 'e':
			enc = optarg;
			break;
		case 'd':
			dec = optarg;
			break;
		case 'h':
			help = true;
			break;
		case ':':
			// option requires operand, but none given.
			fprintf(stderr, "error: option -%c requires operand\n", optopt);
			errcount++;
			break;
		case '?':
			fprintf(stderr, "error: unrecognized option: -%c\n", optopt);
			errcount++;
			break;
		}
	}

	if (errcount > 0) {
		exit(EXIT_FAILURE);
	}

	if (help) {
		printhelp();
		exit(EXIT_SUCCESS);
	}

	if (enc != NULL) {
		char* a = encode(enc);
		printf("%s: %s\n", enc, a);
		free(a);
	}

	if (dec != NULL) {
		char* a = decode(dec);
		printf("%s: %s\n", dec, a);
		free(a);
	}

	exit(EXIT_SUCCESS);
}
