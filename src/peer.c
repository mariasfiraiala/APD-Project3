#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "utils.h"
#include "peer.h"
#include "send_recv.h"
#include "debug.h"

static void *download_thread_func(void *arg)
{
    struct client_t *c = (struct client_t *) arg;

    MPI_Send(&c->wanted_files, 1, MPI_INT, TRACKER_RANK, 0, MPI_COMM_WORLD);
    for (int i = 0; i < c->wanted_files; ++i) {
        MPI_Send(c->w_files[i], MAX_FILENAME + 1, MPI_CHAR, TRACKER_RANK, 0, MPI_COMM_WORLD);

        struct swarm_t s;
        receive_swarm(&s, TRACKER_RANK);

#ifdef DEBUG
        char file[MAX_FILENAME + 1];
        snprintf(file, MAX_FILENAME + 1, "swarm%d.txt", c->rank);
        FILE *out = fopen(file, "a");
        print_swarm(&s, out);
#endif

        request_missing_chunks(c, &s);
    }

    return NULL;
}

static void *upload_thread_func(void *arg)
{
    struct client_t *c = (struct client_t *) arg;

    while(c->running) {
        MPI_Status status;
        char hash[HASH_SIZE + 1];
        MPI_Recv(hash, HASH_SIZE + 1, MPI_CHAR, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);

        int ack = 1;
        MPI_Send(&ack, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
    }

    return NULL;
}

struct client_t *init(int rank)
{
    struct client_t *c = read_file(rank);
    c->rank = rank;
    c->running = 1;
    memset(c->load, 0, MAX_CLIENTS * sizeof(*c->load));

    MPI_Send(&c->owned_files, 1, MPI_INT, TRACKER_RANK, 0, MPI_COMM_WORLD);
    for (int i = 0; i < c->owned_files; ++i) {
        send_file(&c->o_files[i], TRACKER_RANK);
    }

    int ack;
    MPI_Recv(&ack, 1, MPI_INT, TRACKER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    return c;
}

void peer(int numtasks, int rank)
{
    pthread_t download_thread;
    pthread_t upload_thread;
    void *status;
    int r;

    struct client_t *c = init(rank);

    r = pthread_create(&download_thread, NULL, download_thread_func, (void *) c);
    DIE(r, "pthread_create() failed");

    r = pthread_create(&upload_thread, NULL, upload_thread_func, (void *) c);
    DIE(r, "pthread_create() failed");

    r = pthread_join(download_thread, &status);
    DIE(r, "pthread_join() failed");

    r = pthread_join(upload_thread, &status);
    DIE(r, "pthread_join() failed");
}
