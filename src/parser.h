#ifndef PARSER_H
#define PARSER_H

#include "utils.h"

/**
 * Initializez client with input read from its file.
 *
 * @param rank
 *	Rank of the process, used for opening the right input file
 * @return
 *	Pointer to client structure containing all the useful info
 */
struct client_t *read_file(int rank);

#endif /* PARSER_H */