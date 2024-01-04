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
    int load[MAX_CLIENTS];
    int running;
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

void insert_client(struct swarm_client_t *c, struct file_segments_t *s);

int find_client(struct swarm_t *s, int rank);

void create_client(struct swarm_client_t *c, struct file_t *f, int rank);

void insert_swarm(struct swarm_t *s, struct file_t *f, int rank);

int eq_swarm(struct swarm_t *s, char *f);

int find_swarm(struct tracker_t *t, char *f);

void create_swarm(struct swarm_t *s, struct file_t *f, int rank);

void insert_tracker(struct tracker_t *t, struct file_t *f, int rank);

void get_init_files(struct tracker_t *t, int rank);

void get_send_request_files(struct tracker_t *t, int rank);

int empty_string(char *s);

int get_missing_chunks(struct client_t *c, struct file_meta_t *f, int *chunks);

int get_chunks_client(struct swarm_t *s, int index, int *clients);

int get_best_peer(int *load, int *clients, int cl);

char *get_hash(struct swarm_t *s, int rank, int index);

void request_missing_chunks(struct client_t *c, struct swarm_t *s);

#endif
