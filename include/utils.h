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
#define MOD 10
#define TAG_INIT 0
#define TAG_SWARM 1
#define TAG_DOWNLOAD 2
#define TAG_DOWNLOAD_ACK 3
#define TAG_UPDATE 4
#define TAG_FINISH_FILE 5
#define TAG_FINISH_CLIENT 6
#define TAG_STOP 7

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
    int segments[MAX_CHUNKS];
};

struct full_file_t {
    struct file_meta_t meta;
    char segments[MAX_CHUNKS][HASH_SIZE + 1];
};

struct sparse_file_t {
    struct file_meta_t meta;
    struct file_segments_t chunks;
};

struct client_t {
    int rank;
    int owned_files;
    int wanted_files;
    struct full_file_t o_files[MAX_FILES];
    struct sparse_file_t w_files[MAX_FILES];
    int load[MAX_CLIENTS];
    int received_chunks;
};

struct swarm_client_t {
    int rank;
    struct file_segments_t chunks;
};

struct swarm_t {
    struct full_file_t file;
    int size;
    struct swarm_client_t clients[MAX_CLIENTS];
};

struct tracker_t {
    int nr_swarms;
    struct swarm_t swarms[MAX_FILES];
};

/**
 * Populates tracker with owned files from each seed.
 *
 * @param t
 *	Tracker to be populated
 * @param rank
 *  Sender rank
 * @return
 *	No return
 */
void receive_init_files(struct tracker_t *t, int rank);

/**
 * Receives and sends back swarms.
 *
 * @param t
 *	Tracker that receives and sends back swarms
 * @param rank
 *  Client rank
 * @return
 *	No return
 */
void receive_swarm_request(struct tracker_t * t, int rank);

/**
 * Receives and sends back updated files.
 *
 * @param t
 *	Tracker that receives and sends back files
 * @param rank
 *  Sender rank
 * @return
 *	No return
 */
void receive_update_file(struct tracker_t *t, int rank);

/**
 * Requests missing chunks from peers.
 *
 * @param c
 *	Client that needs swarms / segmentss
 * @param s
 *  Swarm that contains info about the incomplete file
 * @return
 *	No return
 */
void request_missing_chunks(struct client_t *c, struct swarm_t *s);

#endif
