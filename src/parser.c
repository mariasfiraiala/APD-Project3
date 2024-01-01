#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "utils.h"

int parse_by_whitespace(char *buf, char **argv)
{
	int argc = 0;
	for (char *p = strtok(buf, " \t\n"); p; p = strtok(NULL, " \t\n"))
		argv[argc++] = p;

	return argc;
}

char *create_file_name(int rank)
{
	char *file = malloc(MAX_FILENAME);
	snprintf(file, MAX_FILENAME, "in%d.txt", rank);

	return file;
}

void read_file(int rank)
{
	FILE *in = fopen(create_file_name(rank), "r");
	struct client_t *client = malloc(sizeof(*client));

	fscanf(in, "%d\n", &client->owned_files);

	for (int i = 0; i < client->owned_files; ++i) {
		char buff[MAX_LINE], *argv[MAX_LINE];
		fgets(buff, MAX_LINE, in);

		int argc = parse_by_whitespace(buff, argv);
		snprintf(client->o_files[i].name, MAX_FILENAME + 1, "%s", argv[0]);
		client->o_files[i].nr_segments = atoi(argv[1]);

		for (int j = 0; j < client->o_files[i].nr_segments; ++j) {
			fgets(buff, MAX_LINE, in);
			argc = parse_by_whitespace(buff, argv);
			snprintf(client->o_files[i].segments[j], HASH_SIZE + 1, "%s", argv[0]);		
		}
	}

	fscanf(in, "%d\n", &client->wanted_files);

	for (int i = 0; i < client->wanted_files; ++i) {
		char buff[MAX_LINE], *argv[MAX_LINE];
		fgets(buff, MAX_LINE, in);
		int argc = parse_by_whitespace(buff, argv);
		snprintf(client->w_files[i], MAX_FILENAME + 1, "%s", argv[0]);
	}
}
