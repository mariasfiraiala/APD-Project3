#ifndef UTILS_H
#define UTILS_H

#include <errno.h>

#define TRACKER_RANK 0
#define MAX_FILES 10
#define MAX_FILENAME 15
#define HASH_SIZE 32
#define MAX_CHUNKS 100
#define MAX_LINE 100
#define MAX_CLIENTS 7

#define DIE(assertion, call_description)                                       \
    do {                                                                       \
        if (assertion) {                                                       \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                 \
            perror(call_description);                                          \
            exit(errno);                                                       \
        }                                                                      \
    } while (0)

struct file_meta_t {
    char name[MAX_FILENAME + 1];
    int size;
};

struct file_segments_t {
    int nr_segments;
    char segments[MAX_CHUNKS][HASH_SIZE + 1];
};

struct file_t {
    struct file_meta_t meta;
    struct file_segments_t segments;
};

struct client_t {
    int rank;
    int owned_files;
    int wanted_files;
    struct file_t o_files[MAX_FILES];
    char w_files[MAX_FILES][MAX_FILENAME + 1];
};

struct swarm_client_t {
    int rank;
    struct file_segments_t segments;
};

struct swarm_t {
    struct file_meta_t file_meta;
    int size;
    struct swarm_client_t clients[MAX_CLIENTS];
};

struct tracker_t {
    int size;
    struct swarm_t swarms[MAX_FILES];
};

#endif
