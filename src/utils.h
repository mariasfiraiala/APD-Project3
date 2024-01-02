#ifndef UTILS_H
#define UTILS_H

#include <errno.h>

#define TRACKER_RANK 0
#define MAX_FILES 10
#define MAX_FILENAME 15
#define HASH_SIZE 32
#define MAX_CHUNKS 100
#define MAX_LINE 100

#define DIE(assertion, call_description)                                       \
    do {                                                                       \
        if (assertion) {                                                       \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                 \
            perror(call_description);                                          \
            exit(errno);                                                       \
        }                                                                      \
    } while (0)

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

#endif
