#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>
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
	char xored[ENCODE_SIZE];

	xor(xored, src);

	base64_encodestate state;

	// Beginning of encoding.
	base64_init_encodestate(&state);
	// Using dst to up the pointer location.
	dst += base64_encode_block(xored, strlen(xored), dst, &state);
	dst += base64_encode_blockend(dst, &state);

	*dst = 0;
}

/*
 * decode will decode the src string into dst by first decoding base64,
 * then xoring every character with the magic number.
 */
static void decode(char* dst, const char* src) {
	char decoded[ENCODE_SIZE];
	char* ptr_dec = decoded;

	base64_decodestate state;
	// Beginning of decoding.
	base64_init_decodestate(&state);
	int cnt = base64_decode_block(src, strlen(src), ptr_dec, &state);
	printf("decoded to %d bytes\n", cnt);
	ptr_dec += cnt;
	*ptr_dec = '\0';

	xor(dst, decoded);
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
	printf("Usage: xor [-e,-d|-h]\n"
	       "Encodes or decodes Websphere obfuscated strings.\n"
	       "\n"
	       "  -h          prints this help and exits\n"
	       "  -e STRING   encodes the given string\n"
	       "  -d STRING   decodes the given string\n"
	       "\n"
	       "Examples:\n"
	       "\n"
	       "$ xor -e \"hello world\"\n"
	       "hello world: NzozMzB/KDAtMzs=\n"
	       "$ xor -d \"NzozMzB/KDAtMzs=\"\n"
	       "NzozMzB/KDAtMzs=: hello world\n"
	       "\n"
	       "Full source and docs at <http://github.com/krpors/xor>\n");
}

int main(int argc, char* argv[]) {
	int opt = -1;
	char* enc = NULL;
	char* dec = NULL;
	bool help = false;
	int errcount = 0;

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
		char encoded[ENCODE_SIZE];
		encode(encoded, enc);
		printf("%s: %s\n", enc, encoded);
	}

	if (dec != NULL) {
		char decoded[ENCODE_SIZE];
		decode(decoded, dec);
		printf("%s: %s\n", dec, decoded);
	}

	exit(EXIT_SUCCESS);
}
