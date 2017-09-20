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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <strings.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "transform.h"

transform_spec_t *spec = NULL;
transform_spec_t **spec_next = &spec;

#define OPS_NR 5

int file_access_mode = PROT_NONE; ///< PROT_WRITE, PROT_READ

struct {
	const char* name;
	int code;
	int flags; ///< MUST_READ, MUST_WRITE
} ops[OPS_NR] = {
	{ .name = "set",  .code = TRANSFORM_SET,  .flags = PROT_WRITE },
	{ .name = "xor",  .code = TRANSFORM_XOR,  .flags = PROT_READ | PROT_WRITE },
	{ .name = "or",   .code = TRANSFORM_OR ,  .flags = PROT_READ | PROT_WRITE },
	{ .name = "and",  .code = TRANSFORM_AND,  .flags = PROT_READ | PROT_WRITE },
	{ .name = "read", .code = TRANSFORM_READ, .flags = PROT_READ },
};

static int
add_transform_spec(const char* opname, const char* offsetstring, const char* hexstring)
{
	// decode op
	int op = -1;
	for (size_t i = 0; i < OPS_NR; i++) {
		if (!strcasecmp(ops[i].name, opname)) {
			op = ops[i].code;
			file_access_mode |= ops[i].flags;
			break;
		}
	}
	if (op < 0) {
		fprintf(stderr, "Unknown op: %s\n", opname);
		return 1;
	}

	// decode offset
	char* endptr;
	errno = 0;
	unsigned long long offset = strtoull(offsetstring, &endptr, 0);
	if (errno || *endptr || endptr ==  offsetstring) {
		fprintf(stderr, "Invalid offset: %s\n", offsetstring);
		return 1;
	}

	size_t hex_pairs_nr = 0;

	if (op == TRANSFORM_READ) {
		hex_pairs_nr = strtoull(hexstring, &endptr, 0);
		if (errno || *endptr || endptr ==  hexstring) {
			fprintf(stderr, "Invalid read length: %s\n", hexstring);
			return 1;
		}
	} else {
		// validate hexstring
		const char* seeker = hexstring;
		while (*seeker) {
			if (!seeker[1]) {
				fprintf(stderr, "Invalid change specification: hex string has odd number of digits: %s\n", hexstring);
				return 1;
			}
			if (hex_to_int(seeker[0]) < 0
			    || hex_to_int(seeker[1]) < 0) {
				fprintf(stderr, "Invalid change specification: not a tuple of hex digits %c%c\n", seeker[0], seeker[1]);
				return 1;
			}
			seeker += 2;
			hex_pairs_nr += 1;
		}
	}

	// encode
	transform_spec_t* spec = calloc(1, sizeof(transform_spec_t));
	*spec_next = spec;
	spec_next = &spec->next;
	spec->offset = offset;
	spec->transform_op = op;
	spec->hex_string = hexstring;
	spec->hex_string_pairs_nr = hex_pairs_nr;

	return 0;
}

static int
do_transform(const char* filename)
{
	int mmap_prot = file_access_mode;

	// open
	int fd = open(filename, (mmap_prot & PROT_WRITE) ? O_RDWR : O_RDONLY);
	if (fd == -1) {
		perror(filename);
		return 1;
	}

	// get size
	struct stat fd_stat;
	if (fstat(fd, &fd_stat)) {
		perror("fstat()");
		close(fd);
		return 1;
	}
	size_t length = fd_stat.st_size;

	// mmap
	void* mem = mmap(NULL, length, mmap_prot, MAP_SHARED, fd, 0);
	if (mem == MAP_FAILED) {
		perror("mmap()");
		return 1;
	}

	// now transform
	transform(mem, length, spec);

	// release resources
	if (munmap(mem, length)) {
		perror("munmap()");
		return 1;
	}
	if (close(fd)) {
		perror("close()");
		return 1;
	}
	return 0;
}

static void
free_specs(void)
{
	while (spec) {
		transform_spec_t* next = spec->next;
		free(spec);
		spec = next;
	}
}

static void
print_help(void)
{
	printf("Usage:\n  binpatch <filename> (<op> <offset> <change>)*\n");
	printf("Usage:\n  binpatch <filename> READ <offset> <length>\n");
	printf("Performs byte-level modifications in a file.\n");
	printf("op:\t SET, XOR, AND, OR\n");
	printf("offset:\t File offset (hex or decimal)\n");
	printf("change:\t Hex string \n");
	printf("\nExample:\n  binpatch file.bin set 0 ac45 xor 0x400 ffffffffffffffffffffffff\n");
	printf("  binpatch file.bin read 0 0x20\n");
}

static void
print_version(void)
{
	printf("binpatch, version %s\n", PACKAGE_VERSION);
}

int
main(int argc, char** argv)
{
	char c;
	while ((c = getopt(argc, argv, "hv")) >= 0) {
		switch (c) {
		case 'h':
			print_help();
			return EXIT_SUCCESS;

		case 'v':
			print_version();
			return EXIT_SUCCESS;

		default:
			print_help();
			return EXIT_FAILURE;
		}
	}

	if (optind >= argc) {
		print_help();
		return EXIT_FAILURE;
	}
	const char* filename = argv[optind++];
	while (optind < argc) {
		if (optind + 2 >= argc) {
			fprintf(stderr, "Incomplete trailing transformation triple after '%s'?\n", argv[optind - 1]);
			print_help();
			return EXIT_FAILURE;
		}
		if (add_transform_spec(argv[optind], argv[optind + 1], argv[optind + 2])) {
			free_specs();
			return EXIT_FAILURE;
		}
		optind += 3;
	}
	int result = do_transform(filename);

	free_specs();
	return result;
}
