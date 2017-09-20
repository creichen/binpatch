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

#ifndef _BINPATCH_TRANSFORM_H
#define _BINPATCH_TRANSFORM_H

#include <stdlib.h>

#define TRANSFORM_SET  1
#define TRANSFORM_XOR  2
#define TRANSFORM_AND  3
#define TRANSFORM_OR   4
#define TRANSFORM_READ 5

/**
 * @brief Transformation specification.
 */
typedef struct transform_spec {
	unsigned long long offset;
	const char* hex_string;      ///< string of hex pairs, e.g., "f32a"
	size_t hex_string_pairs_nr;  ///< number of hex digit pairs in hex_string
	int transform_op;            ///< One of TRANSFORM_*
	struct transform_spec* next; ///< optional next command to run
} transform_spec_t;

/**
 * @brief Transforms the given memory region according to the spec.
 *
 * @param dest Memory region to transform.
 * @param byte_size Extent of the memory region (excessive writes are clipped)
 * @param spec Transformation spec to apply.
 */
void
transform(void* dest, size_t byte_size,
	  transform_spec_t *spec);


/**
 * @brief Parses a hex character.
 *
 * @param[in] c The character to parse.
 * @return The hexadecimal value, or -1 if c does not contain a hex character.
 */
char
hex_to_int(char c);

#endif
