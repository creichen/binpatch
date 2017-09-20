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
#include <string.h>

#include "transform.h"

int overall_result = 0;

static void
check_range(unsigned char* dest, unsigned char* expected, size_t len,
	    const char* name)
{
	int mismatch = 0;
	for (size_t pos = 0; pos < len; pos++) {
		if (dest[pos] != expected[pos]) {
			mismatch = 1;
			break;
		}
	}
	fprintf(stderr, "Test: %s ", name);
	if (mismatch) {
		fprintf(stderr, "FAILURE\n");
		fprintf(stderr, "Actual:  ");
		for (size_t pos = 0; pos < len; pos++) {
			fprintf(stderr, " %02x", dest[pos]);
		}
		fprintf(stderr, "\n");
		fprintf(stderr, "Expected:");
		for (size_t pos = 0; pos < len; pos++) {
			fprintf(stderr, " %02x", expected[pos]);
		}
		fprintf(stderr, "\n");
		overall_result = 1;
	} else {
		fprintf(stderr, "OK\n");
	}
}

#define BASE_EXAMPLE_LEN 16
unsigned char base_example[BASE_EXAMPLE_LEN] = {
	0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf
};

void
test_no_op(void)
{
	size_t len = 8;
	unsigned char data[BASE_EXAMPLE_LEN];
	memcpy(data, base_example + 3, len);
	transform(data, len, NULL);
	check_range(data, base_example + 3, len, "no_op");
}

void
test_set_zero(void)
{
	size_t len = 5;
	size_t offset = 0;
	unsigned char data[BASE_EXAMPLE_LEN];
	memcpy(data, base_example + offset, len);
	transform_spec_t t0 = {
		.offset = 1,
		.hex_string = "f2",
		.hex_string_pairs_nr = 0,
		.transform_op = TRANSFORM_SET,
		.next = NULL
	};
	transform(data, len, &t0);

	check_range(data, base_example + offset, len, "set_zero");
}


void
test_set_one(void)
{
	size_t len = 5;
	size_t offset = 0;
	unsigned char data[BASE_EXAMPLE_LEN];
	memcpy(data, base_example + offset, len);
	transform_spec_t t0 = {
		.offset = 1,
		.hex_string = "f2",
		.hex_string_pairs_nr = 1,
		.transform_op = TRANSFORM_SET,
		.next = NULL
	};
	transform(data, len, &t0);

	unsigned char expected[] = { 0xa0, /* */ 0xf2, 0xa2, 0xa3, 0xa4, 0xa5 };

	check_range(data, expected, len, "set_one");
}


void
test_set_three(void)
{
	size_t len = 6;
	size_t offset = 1;
	unsigned char data[BASE_EXAMPLE_LEN];
	memcpy(data, base_example + offset, len);
	transform_spec_t t0 = {
		.offset = 1,
		.hex_string = "aabbcc",
		.hex_string_pairs_nr = 3,
		.transform_op = TRANSFORM_SET,
		.next = NULL
	};
	transform(data, len, &t0);

	unsigned char expected[] = { 0xa1, 0xaa, 0xbb, 0xcc, 0xa5, 0xa6 };

	check_range(data, expected, len, "set_three");
}


void
test_xor_three(void)
{
	size_t len = 5;
	size_t offset = 0;
	unsigned char data[BASE_EXAMPLE_LEN];
	memcpy(data, base_example + offset, len);
	transform_spec_t t0 = {
		.offset = 0,
		.hex_string = "ff1723",
		.hex_string_pairs_nr = 3,
		.transform_op = TRANSFORM_XOR,
		.next = NULL
	};
	transform(data, len, &t0);

	unsigned char expected[] = { 0x5f, 0xb6, 0x81, 0xa3, 0xa4, 0xa5 };

	check_range(data, expected, len, "xor_three");
}

void
test_and_two(void)
{
	size_t len = 5;
	size_t offset = 0;
	unsigned char data[BASE_EXAMPLE_LEN];
	memcpy(data, base_example + offset, len);
	transform_spec_t t0 = {
		.offset = 1,
		.hex_string = "3212",
		.hex_string_pairs_nr = 2,
		.transform_op = TRANSFORM_AND,
		.next = NULL
	};
	transform(data, len, &t0);

	unsigned char expected[] = { 0xa0, 0x20, 0x02, 0xa3, 0xa4, 0xa5 };

	check_range(data, expected, len, "and_two");
}


void
test_or_two(void)
{
	size_t len = 5;
	size_t offset = 0;
	unsigned char data[BASE_EXAMPLE_LEN];
	memcpy(data, base_example + offset, len);
	transform_spec_t t0 = {
		.offset = 1,
		.hex_string = "3311",
		.hex_string_pairs_nr = 2,
		.transform_op = TRANSFORM_OR,
		.next = NULL
	};
	transform(data, len, &t0);

	unsigned char expected[] = { 0xa0, 0xb3, 0xb3, 0xa3, 0xa4, 0xa5 };

	check_range(data, expected, len, "or_two");
}

void
test_multi(void)
{
	size_t len = 5;
	size_t offset = 0;
	unsigned char data[BASE_EXAMPLE_LEN];
	memcpy(data, base_example + offset, len);
	transform_spec_t t1 = {
		.offset = 2,
		.hex_string = "34",
		.hex_string_pairs_nr = 1,
		.transform_op = TRANSFORM_XOR,
		.next = NULL
	};
	transform_spec_t t0 = {
		.offset = 1,
		.hex_string = "111111",
		.hex_string_pairs_nr = 3,
		.transform_op = TRANSFORM_SET,
		.next = &t1
	};
	transform(data, len, &t0);

	unsigned char expected[] = { 0xa0, 0x11, 0x25, 0x11, 0xa4 };

	check_range(data, expected, len, "multi");
}

void
test_clip(void)
{
	size_t len = 5;
	size_t offset = 0;
	unsigned char data[BASE_EXAMPLE_LEN];
	memcpy(data, base_example + offset, len + 3);
	transform_spec_t t0 = {
		.offset = 4,
		.hex_string = "11223344",
		.hex_string_pairs_nr = 1,
		.transform_op = TRANSFORM_SET,
		.next = NULL
	};
	transform(data, len, &t0);

	unsigned char expected[] = { 0xa0, 0xa1, 0xa2, 0xa3, 0x11, 0xa5, 0xa6, 0xa7, 0xa8 };

	check_range(data, expected, len + 3, "clip");
}

void
test_clip2(void)
{
	size_t len = 5;
	size_t offset = 0;
	unsigned char data[BASE_EXAMPLE_LEN];
	memcpy(data, base_example + offset, len + 3);
	transform_spec_t t0 = {
		.offset = 7,
		.hex_string = "11223344",
		.hex_string_pairs_nr = 1,
		.transform_op = TRANSFORM_SET,
		.next = NULL
	};
	transform(data, len, &t0);

	unsigned char expected[] = { 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8 };

	check_range(data, expected, len + 3, "clip2");
}

void
test_hexchar(void)
{
	size_t len = 8;
	size_t offset = 0;
	unsigned char data[BASE_EXAMPLE_LEN];
	memcpy(data, base_example + offset, len);
	transform_spec_t t0 = {
		.offset = 0,
		.hex_string = "001199aaffAAEeFF",
		.hex_string_pairs_nr = 8,
		.transform_op = TRANSFORM_SET,
		.next = NULL
	};
	transform(data, len, &t0);

	unsigned char expected[] = { 0x00, 0x11, 0x99, 0xaa, 0xff, 0xaa, 0xee, 0xff };

	check_range(data, expected, len, "hexchar");
}

int
main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	test_no_op();
	test_set_zero();
	test_set_one();
	test_set_three();
	test_xor_three();
	test_and_two();
	test_or_two();
	test_multi();
	test_clip();
	test_clip2();
	test_hexchar();

	return overall_result;
}
