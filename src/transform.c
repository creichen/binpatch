/***************************************************************************
  Copyright (C) 2017 Christoph Reichenbach


 This program may be modified and copied freely according to the terms of
 the GNU general public license (GPL), as long as the above copyright
 notice and the licensing information contained herein are preserved.

 Please refer to www.gnu.org for licensing details.

 This work is provided AS IS, without warranty of any kind, expressed or
 implied, including but not limited to the warranties of merchantability,
 noninfringement, and fitness for a specific purpose. The author will not
 be held liable for any damage caused by this work or derivatives of it.

 By using this source code, you agree to the licensing terms as stated
 above.


 Please contact the maintainer for bug reports or inquiries.

 Current Maintainer:

    Christoph Reichenbach (CR) <creichen@gmail.com>

***************************************************************************/

#include <stdio.h>

#include "transform.h"

char
hex_to_int(char c)
{
	if (c >= '0' && c <= '9') {
		return (c - '0');
	}
	if (c >= 'a' && c <= 'f') {
		return 10 + (c - 'a');
	}
	if (c >= 'A' && c <= 'F') {
		return 10 + (c - 'A');
	}
	return -1;
}

static unsigned char
hexpair_to_int(const char*src)
{
	return (((int) hex_to_int(src[0])) << 4)
		| hex_to_int(src[1]);
}

void
transform(void* dest_, size_t size,
	  transform_spec_t *spec)
{
	unsigned char* dest = (unsigned char*) dest_;
	while (spec) {
		size_t offset = spec->offset;
		if (offset >= size) {
			spec = spec->next;
			continue;
		}

		size_t spec_size = spec->hex_string_pairs_nr;
		if (spec_size + spec->offset >= size) {
			spec_size = size - offset;
		}

		const char* hex_string = spec->hex_string;

		int read_only = spec->transform_op == TRANSFORM_READ;

		for (size_t i = 0; i < spec_size; ++i) {
			unsigned char byte = read_only? 0 : hexpair_to_int(hex_string + (i << 1));

			switch (spec->transform_op) {
			case TRANSFORM_SET:
				dest[i + offset] = byte;
				break;
			case TRANSFORM_XOR:
				dest[i + offset] ^= byte;
				break;
			case TRANSFORM_OR:
				dest[i + offset] |= byte;
				break;
			case TRANSFORM_AND:
				dest[i + offset] &= byte;
				break;
			case TRANSFORM_READ:
				printf("%02x", dest[i + offset]);
				break;
			}
		}
		if (read_only) {
			puts("");
		}

		spec = spec->next;
	}
}
