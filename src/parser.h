#ifndef PARSER_H
#define PARSER_H

#include "utils.h"

struct file_t {
	char name[MAX_FILENAME + 1];
	int nr_segments;
	char segments[MAX_CHUNKS][HASH_SIZE + 1];
};

struct client_t {
	int owned_files;
	int wanted_files;
	struct file_t o_files[MAX_FILES];
	char w_files[MAX_FILES][MAX_FILENAME + 1];
};

int parse_by_whitespace(char *buf, char **argv);

char *create_file_name(int rank);

void read_file(int rank);

#endif