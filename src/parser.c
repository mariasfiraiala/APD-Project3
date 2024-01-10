#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "utils.h"

/**
 * Parses by whitespaces.
 *
 * @param buf
 *	Buffer to be parsed
 * @param argv
 *	List of non-whitespace strings
 * @return
 *	Number of non-whitespace strings
 */
static int parse_by_whitespace(char *buf, char **argv)
{
	int argc = 0;
	for (char *p = strtok(buf, " \t\n"); p; p = strtok(NULL, " \t\n"))
		argv[argc++] = p;

	return argc;
}

/**
 * Creates input file name based on client rank.
 *
 * @param rank
 *	The process rank
 * @return
 *	String containing the file name
 */
static char *create_file_name(int rank)
{
	char *file = malloc(MAX_FILENAME + 1);
	DIE(!file, "malloc() failed");
	snprintf(file, MAX_FILENAME + 1, "in%d.txt", rank);

	return file;
}

struct client_t *read_file(int rank)
{
	struct client_t *client = calloc(1, sizeof(*client));
	DIE(!client, "calloc() failed");

	FILE *in = fopen(create_file_name(rank), "r");
	fscanf(in, "%d\n", &client->owned_files);

	for (int i = 0; i < client->owned_files; ++i) {
		char buff[MAX_LINE], *argv[MAX_LINE];
		fgets(buff, MAX_LINE, in);

		parse_by_whitespace(buff, argv);
		snprintf(client->o_files[i].meta.name, MAX_FILENAME + 1, "%s", argv[0]);
		client->o_files[i].meta.size = atoi(argv[1]);

		for (int j = 0; j < client->o_files[i].meta.size; ++j) {
			fgets(buff, MAX_LINE, in);
			parse_by_whitespace(buff, argv);
			snprintf(client->o_files[i].segments[j], HASH_SIZE + 1, "%s", argv[0]);		
		}
	}

	fscanf(in, "%d\n", &client->wanted_files);

	for (int i = 0; i < client->wanted_files; ++i) {
		char buff[MAX_LINE], *argv[MAX_LINE];
		fgets(buff, MAX_LINE, in);
		parse_by_whitespace(buff, argv);
		snprintf(client->w_files[i].meta.name, MAX_FILENAME + 1, "%s", argv[0]);
	}

	return client;
}
