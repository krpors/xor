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

static void encode(char* src, const char* dst);
static void decode(char* src, const char* dst);
static void    xor(char* dst, const char* src);

/*
 * encode will encode the src string into dst by xoring every character with
 * the magic number, then converted to base64.
 */
static void encode(char* dst, const char* src) {
	char* xored = malloc(ENCODE_SIZE * sizeof(char));
	xor(xored, src);

	base64_encodestate state;

	// Beginning of encoding.
	base64_init_encodestate(&state);
	// Using dst to up the pointer location.
	dst += base64_encode_block(xored, strlen(xored), dst, &state);
	dst += base64_encode_blockend(dst, &state);

	*dst = 0;

	free(xored);
}

/*
 * decode will decode the src string into dst by first decoding base64,
 * then xoring every character with the magic number.
 */
static void decode(char* dst, const char* src) {
	char* decoded = malloc(ENCODE_SIZE * sizeof(char));
	char* ptr_dec = decoded; // TODO: why do I need an explicit pointer to the `decoded' string?

	base64_decodestate state;
	// Beginning of decoding.
	base64_init_decodestate(&state);
	int cnt = base64_decode_block(src, strlen(src), decoded, &state);
	ptr_dec += cnt;
	*ptr_dec = '\0';

	xor(dst, decoded);

	free(decoded);
}

static void xor(char* dst, const char* src) {
	// iterate over all chars in 'input', xor it with the magic number into dst.
	int i = 0;
	for (; src[i]; i++) {
		dst[i] = src[i] ^ MAGIC;
	}
	// Redundant if dst is large enough, but be sure we add a terminating null byte.
	dst[i] = '\0';
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
		char* encoded = malloc(ENCODE_SIZE * sizeof(char));
		encode(encoded, enc);
		printf("%s: %s\n", enc, encoded);
		free(encoded);
	}

	if (dec != NULL) {
		char* decoded = malloc(ENCODE_SIZE * sizeof(char));
		decode(decoded, dec);
		printf("%s: %s\n", dec, decoded);
		free(decoded);
	}

	exit(EXIT_SUCCESS);
}
